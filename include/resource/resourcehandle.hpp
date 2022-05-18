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

#ifndef XENGINE_RESOURCEHANDLE_HPP
#define XENGINE_RESOURCEHANDLE_HPP

#include <memory>
#include <utility>

#include "resource/uri.hpp"
#include "resource/resourceregistry.hpp"

#include "async/threadpool.hpp"

namespace xengine {
    template<typename T>
    class XENGINE_EXPORT ResourceHandle {
    public:
        ResourceHandle() = default;

        explicit ResourceHandle(Uri uri,
                                std::shared_ptr<ResourceRegistry> registry = nullptr,
                                std::shared_ptr<Resource> resource = nullptr)
                : uri(std::move(uri)), registry(std::move(registry)), resource(std::move(resource)) {
            loadTask = ThreadPool::getPool().addTask([this] {
                if (this->registry) {
                    this->resource = this->registry->get(this->uri);
                } else {
                    this->resource = ResourceRegistry::getDefaultRegistry().get(this->uri);
                }
            });
        }

        ~ResourceHandle() {
            syncWithLoader();
        };

        ResourceHandle(const ResourceHandle &other) = default;

        ResourceHandle(ResourceHandle &&other) noexcept = default;

        ResourceHandle &operator=(const ResourceHandle &other) = default;

        ResourceHandle &operator=(ResourceHandle &&other) noexcept = default;

        bool operator==(const ResourceHandle &other) const {
            return uri == other.uri
                   && resource == other.resource
                   && registry == other.registry;
        }

        const Uri &getUri() const {
            return uri;
        }

        const T &get() const {
            return dynamic_cast<const T &>(*getResource());
        }

        const std::shared_ptr<Resource> &getResource() const {
            syncWithLoader();
            if (!resource) {
                throw std::runtime_error("Failed to load resource");
            }
            return resource;
        }

    private:
        void syncWithLoader() const {
            loadTask->wait();
        }

        Uri uri;
        std::shared_ptr<Resource> resource;
        std::shared_ptr<ResourceRegistry> registry;

        std::shared_ptr<Task> loadTask;
    };
}

#endif //XENGINE_RESOURCEHANDLE_HPP
