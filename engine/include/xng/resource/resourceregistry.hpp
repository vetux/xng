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

#ifndef XENGINE_RESOURCEREGISTRY_HPP
#define XENGINE_RESOURCEREGISTRY_HPP

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <condition_variable>
#include <filesystem>
#include <shared_mutex>

#include "xng/io/archive.hpp"

#include "uri.hpp"
#include "resource.hpp"
#include "resourcebundle.hpp"
#include "resourceimporter.hpp"

#include "xng/async/threadpool.hpp"

#include "xng/util/refcounter.hpp"

namespace xng {
    /**
     * A resource registry is responsible for loading and managing resource data.
     * The registry invokes the set importer for importing data.
     * The registry uses reference counting for resource lifetime management.
     * ResourceHandle can be used to do the reference counting with a RAII interface.
     */
    class XENGINE_EXPORT ResourceRegistry {
    public:
        /**
         * The default registry used by resource handle if no registry is specified.
         *
         * Users must set importers and schemes with their archive instances before instantiating resource handles referencing the schemes.
         *
         * @return
         */
        static ResourceRegistry &getDefaultRegistry();

        ResourceRegistry();

        ~ResourceRegistry();

        /**
         * Set the archive instances with their corresponding scheme to use for resolving the uri file paths.
         *
         * @param scheme The scheme under which the archive should be made available.
         * @param archive
         */
        void addArchive(const std::string &scheme, std::shared_ptr<Archive> archive);

        void removeArchive(const std::string &scheme);

        void setImporters(std::vector<std::unique_ptr<ResourceImporter> > importers);

        const std::vector<std::unique_ptr<ResourceImporter> > &getImporters();

        ResourceImporter &getImporter(const std::string &fileExtension);

        Archive &getArchive(const std::string &scheme);

        /**
         * @param scheme The scheme to use when an uri does not specify a scheme.
         */
        void setDefaultScheme(const std::string &scheme) { defaultScheme = scheme; }

        /**
         * If the scheme on uri is not set the registry uses the archive of the default scheme and if no default archive is set it
         * returns the first archive for which the uri file exists otherwise an exception is thrown.
         *
         * @param uri
         * @param typeName
         * @return
         */
        const Resource &get(const Uri &uri) {
            mutex.lock();
            auto it = loadTasks.find(uri.getFile());
            if (it != loadTasks.end()) {
                auto task = it->second;
                mutex.unlock();
                auto ex = task->join();
                if (ex) {
                    std::rethrow_exception(ex);
                }
                mutex.lock();
            }
            if (resources.find(uri) != resources.end()) {
                auto &ret = resources.at(uri);
                mutex.unlock();
                return ret;
            } else {
                auto &ret = bundles.at(uri.getFile()).get(uri.getAsset());
                resources.emplace(uri, ret);
                mutex.unlock();
                return ret;
            }
        }

        void incRef(const Uri &uri);

        void decRef(const Uri &uri);

        void reload(const Uri &uri);

        void reloadAll() {
            for (auto &uri: getUris()) {
                reload(uri);
            }
        }

        void await(const Uri &uri);

        void awaitAll() {
            for (auto &uri: getLoadingUris()) {
                await(uri);
            }
        }

        const std::unordered_set<Uri> &getUris() const;

        std::unordered_set<Uri> getLoadingUris() {
            std::lock_guard<std::mutex> g(mutex);
            return loadingUris;
        }

        bool isLoaded(const Uri &uri) {
            std::lock_guard<std::mutex> g(mutex);
            return bundles.find(uri.getFile()) != bundles.end();
        }

        bool isLoading(const Uri &uri) {
            std::lock_guard<std::mutex> g(mutex);
            return loadTasks.find(uri.getFile()) == loadTasks.end();
        }

    private:
        void load(const Uri &uri);

        void unload(const Uri &uri);

        Archive &resolveUri(const Uri &uri);

        std::mutex mutex;

        std::shared_mutex archiveMutex;

        RefCounter<std::string, unsigned long> bundleRefCounter;

        std::unordered_map<std::string, std::shared_ptr<Task> > loadTasks;

        std::unordered_map<std::string, std::shared_ptr<Archive> > archives;
        std::unordered_map<std::string, ResourceBundle> bundles;
        std::unordered_map<Uri, const Resource &> resources;

        std::string defaultScheme;

        std::shared_mutex importerMutex;
        std::vector<std::unique_ptr<ResourceImporter> > importers;

        std::unordered_set<Uri> uris;
        std::unordered_set<Uri> loadingUris;

        std::unordered_set<std::string> abortedBundleLoads;
    };
}
#endif //XENGINE_RESOURCEREGISTRY_HPP
