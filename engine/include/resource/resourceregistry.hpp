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

#ifndef XENGINE_RESOURCEREGISTRY_HPP
#define XENGINE_RESOURCEREGISTRY_HPP

#include <thread>
#include <memory>
#include <map>
#include <set>
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <shared_mutex>

#include "io/archive.hpp"

#include "resource/uri.hpp"
#include "resource/resource.hpp"
#include "resource/resourcebundle.hpp"
#include "resource/resourceimporter.hpp"

#include "async/threadpool.hpp"

#include "util/refcounter.hpp"

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

        void setImporter(ResourceImporter importer);

        Archive &getArchive(const std::string &scheme);

        template<typename T>
        T &getArchiveT(const std::string &name) {
            return dynamic_cast<T &>(getArchive(name));
        }

        /**
         * @param scheme The scheme to use when an uri does not specify a scheme.
         */
        void setDefaultScheme(const std::string &scheme) { defaultScheme = scheme; }

        /**
         * If the scheme on uri is not set the registry uses the archive of the default scheme and if no default archive is set it
         * returns the first archive for which the uri file exists otherwise an exception is thrown.
         *
         * @param uri
         * @return
         */
        const Resource &get(const Uri &uri);

        void incRef(const Uri &uri);

        void decRef(const Uri &uri);

        void reloadAllResources();

        void awaitImports();

    private:
        void load(const Uri &uri);

        void unload(const Uri &uri);

        const Resource &getData(const Uri &uri);

        Archive &resolveUri(const Uri &uri);

        std::mutex mutex;

        std::shared_mutex archiveMutex;

        RefCounter<std::string, unsigned long> bundleRefCounter;

        std::unordered_map<std::string, std::shared_ptr<Task>> loadTasks;

        std::map<std::string, std::shared_ptr<Archive>> archives;
        std::map<std::string, ResourceBundle> bundles;

        std::string defaultScheme;

        std::shared_mutex importerMutex;
        ResourceImporter importer;
    };
}
#endif //XENGINE_RESOURCEREGISTRY_HPP
