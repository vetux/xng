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
    static std::unique_ptr<ResourceRegistry> defRepo;

    ResourceRegistry &ResourceRegistry::getDefaultRegistry() {
        if (!defRepo) {
            defRepo = std::make_unique<ResourceRegistry>();
        }
        return *defRepo;
    }

    ResourceRegistry::ResourceRegistry(const std::chrono::high_resolution_clock::duration &gcInterval)
            : gcInterval(gcInterval) {
        gcThread = std::thread([this] { gcLoop(); });
    }

    ResourceRegistry::~ResourceRegistry() {
        gcShutdown.store(true);
        gcCondition.notify_all();
        gcThread.join();
    }

    std::shared_ptr<Resource> ResourceRegistry::get(const Uri &uri) {
        return getData(uri);
    }

    void ResourceRegistry::setArchive(std::shared_ptr<Archive> a) {
        archive = std::move(a);
    }

    std::shared_ptr<Resource> ResourceRegistry::getData(const Uri &uri) {
        std::lock_guard<std::mutex> g(gcMutex);
        auto &path = uri.getFile();
        auto it = bundles.find(path);
        if (it == bundles.end()) {
            bundles[path] = loadBundle(path);
        }
        return bundles[path].get(uri.getAsset());
    }

    ResourceBundle ResourceRegistry::loadBundle(const std::string &path) {
        if (!archive) {
            throw std::runtime_error("No archive assigned in repository");
        }
        auto stream = archive->open(path);
        return ResourceImporter().import(*stream);
    }

    void ResourceRegistry::gcLoop() {
        while (!gcShutdown) {
            std::unique_lock<std::mutex> gcLock(gcMutex);
            std::set<std::string> unusedBundles;
            for (auto &pair: bundles) {
                bool bundleUse = false;
                for (auto &ap: pair.second.assets) {
                    if (ap.second.use_count() > 1) {
                        bundleUse = true;
                        break;
                    }
                }
                if (!bundleUse) {
                    unusedBundles.insert(pair.first);
                }
            }
            for (auto &bundle: unusedBundles) {
                bundles.erase(bundle);
            }
            gcCondition.wait_for(gcLock, gcInterval, [this] {
                return gcShutdown.load();
            });
        }
    }
}