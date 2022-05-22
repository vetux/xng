/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "resource/resourceregistry.hpp"

#include "resource/resourceimporter.hpp"

namespace xengine {
    static std::unique_ptr<ResourceRegistry> defRepo = nullptr;

    ResourceRegistry &ResourceRegistry::getDefaultRegistry() {
        if (!defRepo) {
            defRepo = std::make_unique<ResourceRegistry>();
        }
        return *defRepo;
    }

    ResourceRegistry::ResourceRegistry() {
    }

    ResourceRegistry::~ResourceRegistry() {
        for (auto &pair: loadTasks) {
            pair.second->join();
        }
    }

    void ResourceRegistry::setArchive(std::shared_ptr<Archive> a) {
        archive = std::move(a);
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
        auto path = uri.getFile();

        auto it = loadTasks.find(path);
        if (it == loadTasks.end()) {
            loadTasks[path] = ThreadPool::getPool().addTask([this, path]() {
                std::filesystem::path p(path);
                auto stream = archive->open(path);
                auto bundle = ResourceImporter().import(*stream, p.extension(), archive.get());
                std::lock_guard<std::mutex> g(mutex);
                bundles[path] = std::move(bundle);
            });
        }
    }

    void ResourceRegistry::unload(const Uri &uri) {
        auto path = uri.getFile();

        std::shared_ptr<Task> task;
        {
            std::lock_guard<std::mutex> g(mutex);

            auto it = loadTasks.find(path);
            if (it == loadTasks.end()) {
                return;
            } else {
                task = it->second;
            }
        }

        task->join();

        std::lock_guard<std::mutex> g(mutex);
        loadTasks.erase(path);
    }

    const Resource &ResourceRegistry::getData(const Uri &uri) {
        std::lock_guard<std::mutex> g(mutex);
        auto &path = uri.getFile();
        auto it = bundles.find(path);
        if (it == bundles.end()) {
            bundles[path] = loadBundle(path);
        }
        return bundles[path].get(uri.getAsset());
    }

    ResourceBundle ResourceRegistry::loadBundle(const std::filesystem::path &path) {
        if (!archive) {
            throw std::runtime_error("No archive assigned in repository");
        }
        auto stream = archive->open(path);
        return ResourceImporter().import(*stream, path.extension(), archive.get());
    }
}