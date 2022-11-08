/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "xng/resource/resourceregistry.hpp"

#include <utility>

#include "xng/resource/resourceimporter.hpp"
#include "xng/io/archive/memoryarchive.hpp"
#include "xng/log/log.hpp"

namespace xng {
    static std::mutex defMutex;
    static std::unique_ptr<ResourceRegistry> defRepo = nullptr;

    ResourceRegistry &ResourceRegistry::getDefaultRegistry() {
        std::lock_guard<std::mutex> guard(defMutex);
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
            auto ex = pair.second->join();
            if (ex) {
                std::rethrow_exception(ex);
            }
        }
    }

    void ResourceRegistry::addArchive(const std::string &scheme, std::shared_ptr<Archive> archive) {
        std::lock_guard<std::mutex> guard(userMutex);
        std::unique_lock g(archiveMutex);
        if (archives.find(scheme) != archives.end())
            throw std::runtime_error("Archive with scheme " + scheme + " already exists");
        archives[scheme] = std::move(archive);
    }

    void ResourceRegistry::removeArchive(const std::string &scheme) {
        std::lock_guard<std::mutex> guard(userMutex);
        std::unique_lock l(archiveMutex);
        archives.erase(scheme);
    }

    void ResourceRegistry::setImporter(ResourceImporter value) {
        std::lock_guard<std::mutex> guard(userMutex);
        std::unique_lock l(importerMutex);
        importer = std::move(value);
    }

    Archive &ResourceRegistry::getArchive(const std::string &scheme) {
        std::lock_guard<std::mutex> guard(userMutex);
        return *archives.at(scheme);
    }

    const Resource &ResourceRegistry::get(const Uri &uri) {
        return getData(uri);
    }

    void ResourceRegistry::incRef(const Uri &uri) {
        std::lock_guard<std::mutex> guard(userMutex);
        if (bundleRefCounter.inc(uri.getFile())) {
            load(uri);
        }
    }

    void ResourceRegistry::decRef(const Uri &uri) {
        std::lock_guard<std::mutex> guard(userMutex);
        if (bundleRefCounter.dec(uri.getFile())) {
            unload(uri);
        }
    }

    void ResourceRegistry::reloadAllResources() {
        std::lock_guard<std::mutex> guard(userMutex);
        std::vector<std::pair<Uri, std::shared_ptr<Task>>> tasks;
        {
            std::lock_guard<std::mutex> g(mutex);
            for (auto &task: loadTasks)
                tasks.emplace_back(task);
        }
        for (auto &task: tasks)
            task.second->join();
        loadTasks.clear();
        bundles.clear();
        for (auto &task: tasks) {
            load(task.first);
        }
    }

    void ResourceRegistry::awaitImports() {
        userMutex.lock();
        auto tasks = loadTasks;
        userMutex.unlock();
        for (auto &task: tasks) {
            task.second->join();
        }
    }

    void ResourceRegistry::load(const Uri &uri) {
        std::lock_guard<std::mutex> g(mutex);

        auto it = loadTasks.find(uri.getFile());
        if (it == loadTasks.end()) {
            loadTasks[uri.getFile()] = ThreadPool::getPool().addTask([this, uri]() {
                try {
                    std::shared_lock l(importerMutex);

                    auto &archive = resolveUri(uri);
                    std::filesystem::path path(uri.getFile());
                    auto stream = archive.open(path.string());
                    auto bundle = importer.import(*stream, path.extension().string());

                    std::lock_guard<std::mutex> g(mutex);
                    bundles[path.string()] = std::move(bundle);
                } catch (const std::runtime_error &e) {
                    Log::instance().log(ERROR, e.what());
                    throw e;
                }
            });
        }
    }

    void ResourceRegistry::unload(const Uri &uri) {
        std::shared_ptr<Task> task;
        {
            std::lock_guard<std::mutex> g(mutex);

            auto it = loadTasks.find(uri.getFile());
            if (it == loadTasks.end()) {
                return;
            } else {
                task = it->second;
            }
        }

        auto ex = task->join();
        if (ex) {
            std::rethrow_exception(ex);
        }

        std::lock_guard<std::mutex> g(mutex);
        loadTasks.erase(uri.getFile());
    }

    const Resource &ResourceRegistry::getData(const Uri &uri) {
        userMutex.lock();
        mutex.lock();
        auto it = loadTasks.find(uri.getFile());
        if (it == loadTasks.end()) {
            throw std::runtime_error("IncRef not called for bundle pointed at by uri " + uri.toString());
        }
        auto task = loadTasks.at(uri.getFile());
        userMutex.unlock();
        mutex.unlock();
        auto ex = task->join();
        if (ex) {
            std::rethrow_exception(ex);
        }
        userMutex.lock();
        mutex.lock();
        auto &ret = bundles[uri.getFile()].get(uri.getAsset());
        userMutex.unlock();
        mutex.unlock();
        return ret;
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
