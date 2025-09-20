/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_RENDERREGISTRY_HPP
#define XENGINE_RENDERREGISTRY_HPP

namespace xng {
    enum SharedResourceType : int {
        RESOURCE_COMPOSITE_TEXTURES = 0,
        RESOURCE_GEOMETRY_BUFFER,
        RESOURCE_DEFERRED_TEXTURES,
        RESOURCE_FORWARD_TEXTURES,

        RESOURCE_USER = 255
    };

    class SharedResourceBase {
    public:
        virtual ~SharedResourceBase() = default;
    };

    template<SharedResourceType TYPE>
    class SharedResource : public SharedResourceBase {
    public:
        static const inline SharedResourceType resourceType = TYPE;
    };

    /**
     * Stores resources shared between passes in a graph.
     */
    class SharedResourceRegistry {
    public:
        template<typename T>
        void set(const T &value) {
            resources[T::resourceType] = std::make_unique<T>(value);
        }

        template<typename T>
        const T &get() const {
            return down_cast<const T&>(*resources.at(T::resourceType));
        }

        template<typename T>
        bool check() const {
            return resources.find(T::resourceType) != resources.end();
        }

        template<typename T>
        void clear() {
            resources.erase(T::resourceType);
        }

    private:
        std::unordered_map<SharedResourceType, std::unique_ptr<SharedResourceBase> > resources;
    };
}

#endif //XENGINE_RENDERREGISTRY_HPP
