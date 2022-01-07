/**
 *  Mana - 3D Game Engine
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

#ifndef XENGINE_ASSETMANAGER_HPP
#define XENGINE_ASSETMANAGER_HPP

#include "async/threadpool.hpp"

#include "asset/assetpath.hpp"
#include "asset/assetbundle.hpp"
#include "asset/assetimporter.hpp"

namespace xengine {
    //TODO: Implement cross thread error handling
    /**
     * Handles loading asset bundles from disk on a thread pool
     */
    class XENGINE_EXPORT AssetManager {
    public:
        explicit AssetManager(Archive &archive)
                : archive(archive) {}

        void incrementRef(const AssetPath &path) {
            ++bundleReferenceCounters[path.bundle];
            if (bundleTasks.find(path.bundle) == bundleTasks.end()) {
                loadBundle(path.bundle);
            }
        }

        void decrementRef(const AssetPath &path) {
            if (bundleReferenceCounters[path.bundle] <= 0)
                throw std::runtime_error("Bundle reference counter underflow");

            auto ref = --bundleReferenceCounters[path.bundle];
            if (ref == 0) {
                auto it = bundleTasks.find(path.bundle);
                if (it != bundleTasks.end()) {
                    it->second->wait();
                    std::lock_guard<std::mutex> guard(bundlesMutex);
                    bundleTasks.erase(path.bundle);
                    bundles.erase(path.bundle);
                    bundleReferenceCounters.erase(path.bundle);
                }
            }
        }

        template<typename T>
        const T &getAsset(const AssetPath &path) {
            return getBundle(path.bundle).get<T>(path.asset);
        }

        const AssetBundle &getBundle(const std::string &path) {
            auto &counter = bundleReferenceCounters.at(path);
            if (counter <= 0) {
                throw std::runtime_error("getBundle called while bundle bundle reference 0");
            }

            std::unique_lock<std::mutex> guard(bundlesMutex);

            auto it = bundles.find(path);
            if (it != bundles.end()) {
                return it->second;
            } else {
                guard.unlock();

                auto &task = bundleTasks.at(path);
                task->wait();

                guard.lock();

                it = bundles.find(path);
                return it->second;
            }
        }

    private:
        void loadBundle(const std::string &path) {
            auto &pool = ThreadPool::getPool();
            bundleTasks[path] = pool.addTask([this, path]() {
                try {
                    auto bundle = AssetImporter::import(path, archive);
                    std::lock_guard<std::mutex> guard(bundlesMutex);
                    bundles[path] = std::move(bundle);
                } catch (const std::exception &e) { throw e; }
            });
        }

        Archive &archive;
        std::mutex bundlesMutex;
        std::map<std::string, uint> bundleReferenceCounters;
        std::map<std::string, std::shared_ptr<Task>> bundleTasks;
        std::map<std::string, AssetBundle> bundles;
    };
}
#endif //XENGINE_ASSETMANAGER_HPP
