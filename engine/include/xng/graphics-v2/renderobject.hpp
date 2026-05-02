/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_RENDEROBJECT_HPP
#define XENGINE_RENDEROBJECT_HPP

#include <cstddef>

#include "xng/math/transform.hpp"

#include "xng/graphics-v2/stream/bufferstreamer.hpp"
#include "xng/graphics-v2/shadertypes.hpp"

namespace xng {
    /**
     * A handle to a scene object.
     * Currently backed by a shared ptr.
     *
     * This may incur cache misses because the object allocations can be fragmented by the c allocator.
     *
     * In the future this will be replaced by flat arrays stored in the scene object and a custom shared pointer like
     * wrapper around the contiguously allocated objects.
     */
    template<typename T>
    using RenderObjectHandle = std::shared_ptr<T>;

    /**
     * Base class for any render object.
     *
     * Render objects are directly backed by heap resources that are consumed by the renderer.
     */
    class RenderObject {
    public:
        typedef size_t Id;

        enum Type {
            OBJECT_CAMERA,
            OBJECT_TEXTURE,
            OBJECT_MATERIAL,
            OBJECT_SKELETON,
            OBJECT_MESH,
            OBJECT_MODEL,
            OBJECT_POINT_LIGHT,
            OBJECT_DIRECTIONAL_LIGHT,
            OBJECT_SPOT_LIGHT,
            OBJECT_CANVAS,
            OBJECT_PAINT,
        };

        explicit RenderObject(const Id id, const Type type)
            : id(id), type(type) {
        }

        virtual ~RenderObject() = default;

        [[nodiscard]] Id getId() const {
            return id;
        }

        [[nodiscard]] Type getType() const {
            return type;
        }

        /**
         * @return True when this object and all dependencies have no pending uploads.
         */
        virtual bool isUploadComplete() = 0;

        /**
         * Flush the pending uploads of this object and all dependencies.
         *
         * This ensures that the object is available immediately in a Renderer:draw operation but may stall the frame.
         *
         * The Renderer will skip any objects that contain pending uploads.
         */
        virtual void flush() = 0;

    protected:
        Id id;
        Type type;
    };
}

#endif //XENGINE_RENDEROBJECT_HPP
