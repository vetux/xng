/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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
        auto tasks = loadTasks;
        for (auto &pair: tasks) {
            auto ex = pair.second->join();
            if (ex) {
                std::rethrow_exception(ex);
            }
        }
        bundles.clear(); // Deallocate resource bundles which might contain a reference to this registry before destroying the registry object.
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

    void ResourceRegistry::setImporters(std::vector<std::unique_ptr<ResourceImporter>> value) {
        std::unique_lock l(importerMutex);
        importers = std::move(value);
    }

    const std::vector<std::unique_ptr<ResourceImporter>> &ResourceRegistry::getImporters() {
        return importers;
    }

    ResourceImporter &ResourceRegistry::getImporter(const std::string &fileExtension) {
        for (auto &imp: importers) {
            if (imp->getSupportedFormats().find(fileExtension) != imp->getSupportedFormats().end()) {
                return *imp;
            }
        }
        throw std::runtime_error("No importer for extension: " + fileExtension);
    }

    Archive &ResourceRegistry::getArchive(const std::string &scheme) {
        return *archives.at(scheme);
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

    void ResourceRegistry::reload(const Uri &uri) {
        std::shared_ptr<Task> task;
        {
            std::lock_guard<std::mutex> g(mutex);
            auto it = loadTasks.find(uri.getFile());
            if (it != loadTasks.end()) {
                task = it->second;
            }
        }

        if (task != nullptr) {
            task->join();
        }

        bundles.erase(uri.getFile());
        uris.erase(uri);

        load(uri);
    }

    void ResourceRegistry::await(const Uri &uri) {
        std::shared_ptr<Task> task;
        {
            std::lock_guard<std::mutex> g(mutex);
            auto it = loadTasks.find(uri.getFile());
            if (it != loadTasks.end()) {
                task = it->second;
            }
        }

        if (task != nullptr) {
            task->join();
        }
    }

    const std::set<Uri> &ResourceRegistry::getUris() const {
        return uris;
    }

    const std::set<Uri> &ResourceRegistry::getLoadingUris() const {
        return loadingUris;
    }

    void ResourceRegistry::load(const Uri &uri) {
        std::lock_guard<std::mutex> g(mutex);

        uris.insert(uri);

        auto it = loadTasks.find(uri.getFile());
        if (it == loadTasks.end() && bundles.find(uri.getFile()) == bundles.end()) {
            loadingUris.insert(uri);
            loadTasks[uri.getFile()] = ThreadPool::getPool().addTask([this, uri]() {
                try {
                    std::shared_lock l(importerMutex);

                    auto &archive = resolveUri(uri);
                    std::filesystem::path path(uri.getFile());
                    auto stream = archive.open(path.string());
                    auto bundle = getImporter(path.extension().string())
                            .read(*stream, path.extension().string(), path.string(), &archive);

                    std::lock_guard<std::mutex> g(mutex);

                    if (killBundles.find(uri.getFile()) == killBundles.end()) {
                        bundles[uri.getFile()] = std::move(bundle);
                    } else {
                        killBundles.erase(uri.getFile());
                        uris.erase(uri);
                    }

                    loadTasks.erase(uri.getFile());
                    loadingUris.erase(uri);
                } catch (const std::runtime_error &e) {
                    Log::instance().log(ERROR, e.what());

                    throw e;
                }
            });
        }
    }

    void ResourceRegistry::unload(const Uri &uri) {
        std::lock_guard<std::mutex> g(mutex);

        auto it = loadTasks.find(uri.getFile());
        if (it != loadTasks.end()) {
            killBundles.insert(uri.getFile());
        } else {
            uris.erase(uri);
        }
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
