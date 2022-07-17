/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "resource/resourceregistry.hpp"

#include <utility>

#include "resource/resourceimporter.hpp"
#include "io/archive/memoryarchive.hpp"

namespace xng {
    static std::unique_ptr<ResourceRegistry> defRepo = nullptr;

    ResourceRegistry &ResourceRegistry::getDefaultRegistry() {
        if (!defRepo) {
            defRepo = std::make_unique<ResourceRegistry>();
        }
        return *defRepo;
    }

    ResourceRegistry::ResourceRegistry() {
        archives["memory"] = std::make_shared<MemoryArchive>();
    }

    ResourceRegistry::~ResourceRegistry() {
        for (auto &pair: loadTasks) {
            pair.second->join();
        }
    }

    void ResourceRegistry::addArchive(const std::string &scheme, std::shared_ptr<Archive> archive) {
        std::unique_lock g(archiveMutex);
        if (archives.find(scheme) != archives.end())
            throw std::runtime_error("Archive with scheme " + scheme + " already exists");
        archives[scheme] = std::move(archive);
    }

    void ResourceRegistry::removeArchive(const std::string &scheme) {
        std::unique_lock l(archiveMutex);
        archives.erase(scheme);
    }

    void ResourceRegistry::setImporter(ResourceImporter value) {
        std::unique_lock l(importerMutex);
        importer = std::move(value);
    }

    Archive &ResourceRegistry::getArchive(const std::string &scheme) {
        return *archives.at(scheme);
    }

    const Resource &ResourceRegistry::get(const Uri &uri) {
        return getData(uri);
    }

    void ResourceRegistry::incRef(const Uri &uri) {
        if (bundleRefCounter.inc(uri.getFile())) {
            load(uri);
        }
    }

    void ResourceRegistry::decRef(const Uri &uri) {
        if (bundleRefCounter.dec(uri.getFile())) {
            unload(uri);
        }
    }

    void ResourceRegistry::load(const Uri &uri) {
        std::lock_guard<std::mutex> g(mutex);

        auto it = loadTasks.find(uri);
        if (it == loadTasks.end()) {
            loadTasks[uri] = ThreadPool::getPool().addTask([this, uri]() {
                std::shared_lock l(importerMutex);

                auto &archive = resolveUri(uri);
                std::filesystem::path path(uri.getFile());
                auto stream = archive.open(path);
                auto bundle = importer.import(*stream, path.extension());

                std::lock_guard<std::mutex> g(mutex);
                bundles[path] = std::move(bundle);
            });
        }
    }

    void ResourceRegistry::unload(const Uri &uri) {
        std::shared_ptr<Task> task;
        {
            std::lock_guard<std::mutex> g(mutex);

            auto it = loadTasks.find(uri);
            if (it == loadTasks.end()) {
                return;
            } else {
                task = it->second;
            }
        }

        task->join();

        std::lock_guard<std::mutex> g(mutex);
        loadTasks.erase(uri);
    }

    const Resource &ResourceRegistry::getData(const Uri &uri) {
        std::lock_guard<std::mutex> g(mutex);
        auto it = bundles.find(uri.getFile());
        if (it == bundles.end()) {
            bundles[uri.getFile()] = loadBundle(uri);
        }
        return bundles[uri.getFile()].get(uri.getAsset());
    }

    ResourceBundle ResourceRegistry::loadBundle(const Uri &uri) {
        auto &archive = resolveUri(uri);
        std::filesystem::path path(uri.getFile());
        auto stream = archive.open(path);
        return ResourceImporter().import(*stream, path.extension());
    }

    Archive &ResourceRegistry::resolveUri(const Uri &uri) {
        std::shared_lock l(archiveMutex);
        if (uri.getScheme().empty()) {
            if (defaultScheme.empty()) {
                for (auto &a: archives) {
                    if (a.second->exists(uri.getFile())) {
                        return *a.second;
                    }
                }
                throw std::runtime_error("Failed to resolve uri " + uri.toString());
            } else {
                return *archives.at(defaultScheme);
            }
        } else {
            return *archives.at(uri.getScheme());
        }
    }
}