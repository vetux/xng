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

#ifndef XENGINE_RESOURCEHANDLE_HPP
#define XENGINE_RESOURCEHANDLE_HPP

#include <memory>
#include <utility>

#include "uri.hpp"
#include "resourceregistry.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    template<typename T>
    class ResourceHandle : public Messageable {
    public:
        ResourceHandle() = default;

        explicit ResourceHandle(Uri u,
                                ResourceRegistry *r = nullptr)
            : uri(std::move(u)), registry(r) {
            if (!uri.empty()) {
                getRegistry().incRef(uri);
            }
        }

        ~ResourceHandle() override {
            if (!uri.empty()) {
                getRegistry().decRef(uri);
            }
        }

        ResourceHandle(const ResourceHandle<T> &other) {
            uri = other.uri;
            registry = other.registry;
            if (!uri.empty()) {
                getRegistry().incRef(uri);
            }
        }

        ResourceHandle<T> &operator=(const ResourceHandle<T> &other) {
            if (this == &other)
                return *this;

            uri = other.uri;
            registry = other.registry;
            if (!uri.empty()) {
                getRegistry().incRef(uri);
            }

            return *this;
        }

        ResourceHandle(ResourceHandle<T> &&other) noexcept = default;

        ResourceHandle<T> &operator=(ResourceHandle<T> &&other) noexcept = default;

        bool operator==(const ResourceHandle<T> &other) const {
            return uri == other.uri
                   && registry == other.registry;
        }

        bool operator!=(const ResourceHandle<T> &other) const {
            return !(*this == other);
        }

        bool assigned() const {
            return !uri.empty();
        }

        const Uri &getUri() const {
            return uri;
        }

        bool isLoaded() const {
            return getRegistry().isLoaded(uri);
        }

        bool isLoading() const {
            return getRegistry().isLoading(uri);
        }

        const T &get() const {
            return down_cast<const T &>(getRegistry().get(uri, T::typeName));
        }

        ResourceRegistry &getRegistry() const {
            if (registry != nullptr) {
                return *registry;
            } else {
                return ResourceRegistry::getDefaultRegistry();
            }
        }

        Messageable &operator<<(const Message &message) override {
            uri << message.getMessage("uri");
            if (!uri.empty()) {
                getRegistry().incRef(uri);
            }
            return *this;
        }

        Message &operator>>(Message &message) const override {
            auto map = std::map<std::string, Message>();
            uri >> map["uri"];
            message = map;
            return message;
        }

    private:
        Uri uri;
        ResourceRegistry *registry = nullptr;
    };
}

#endif //XENGINE_RESOURCEHANDLE_HPP
