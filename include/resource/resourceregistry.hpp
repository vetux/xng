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

#ifndef XENGINE_RESOURCEREGISTRY_HPP
#define XENGINE_RESOURCEREGISTRY_HPP

#include <thread>
#include <memory>
#include <map>
#include <set>
#include <atomic>
#include <condition_variable>

#include "io/archive.hpp"

#include "resource/uri.hpp"
#include "resource/resource.hpp"
#include "resource/resourcebundle.hpp"

#include "async/threadpool.hpp"

#include "algo/refcounter.hpp"

namespace xengine {
    /**
     * A registry loads asset bundles from an archive and deallocates the bundles from a separate garbage collection thread.
     * The garbage collector uses the shared pointer reference counts in the resource bundles to determine unused bundles.
     */
    class XENGINE_EXPORT ResourceRegistry {
    public:
        /**
         * The default registry used by resource handle if no registry is specified.
         *
         * Users should set the archive instance on the repository before instantiating resource handles.
         *
         * @return
         */
        static ResourceRegistry &getDefaultRegistry();

        ResourceRegistry();

        ~ResourceRegistry();

        /**
         * Set the archive instance to use for resolving the uri file paths.
         *
         * @param archive
         */
        void setArchive(std::shared_ptr<Archive> archive);

        const Resource &get(const Uri &uri);

        void incRef(const Uri &uri);

        void decRef(const Uri &uri);

    private:
        void load(const Uri &uri);

        void unload(const Uri &uri);

        const Resource &getData(const Uri &uri);

        ResourceBundle loadBundle(const std::filesystem::path &path);

        std::mutex mutex;

        RefCounter<std::string, ulong> bundleRefCounter;

        std::map<std::string, std::shared_ptr<Task>> loadTasks;

        std::shared_ptr<Archive> archive;
        std::map<std::string, ResourceBundle> bundles;
    };
}
#endif //XENGINE_RESOURCEREGISTRY_HPP
