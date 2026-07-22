/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_RENDEROBJECT_HPP
#define XENGINE_RENDEROBJECT_HPP

#include <cstring>
#include <algorithm>

namespace xng {
    /**
     * Render object allocations are RAII ref counted via RenderObjectHandle.
     *
     * All render objects types are copy constructable and do not own their allocation state.
     *
     * The scene owns the render object allocation state, and render objects hold a reference to the scene.
     *
     * Thus, all references to objects in a scene must be destroyed before destroying the scene itself.
     */
    class RenderObject {
    public:
        typedef size_t ID;

        static constexpr ID UNASSIGNED_ID = 0;

        enum Type : int {
            RENDER_TEXTURE = 0,
            RENDER_SKELETON,
            RENDER_MESH,
            RENDER_SHADER,
            RENDER_MATERIAL,

            RENDER_MODEL,

            RENDER_CANVAS,
            RENDER_PAINT,

            RENDER_LIGHT_POINT,
            RENDER_LIGHT_DIRECTIONAL,
            RENDER_LIGHT_SPOT,
        };

        virtual ~RenderObject() = default;

        virtual bool isUploadComplete() {
            return true;
        }

        virtual void flush() {
        }
    };

    class RenderObjectRefCounter {
    public:
        virtual ~RenderObjectRefCounter() = default;

        virtual void incrementReference(RenderObject::ID id) = 0;

        virtual void decrementReference(RenderObject::ID id) = 0;
    };

    template<typename T>
    class XENGINE_EXPORT RenderObjectHandle {
    public:
        RenderObjectHandle() = default;

        RenderObjectHandle(RenderObjectRefCounter *refCounter,
                           const RenderObject::ID id,
                           T instance)
            : refCounter(refCounter),
              id(id),
              instance(std::move(instance)) {
            refCounter->incrementReference(id);
        }

        RenderObjectHandle(const RenderObjectHandle &other) {
            refCounter = other.refCounter;
            id = other.id;
            instance = other.instance;

            if (refCounter) {
                refCounter->incrementReference(id);
            }
        }

        RenderObjectHandle &operator=(const RenderObjectHandle &other) {
            if (this == &other) {
                return *this;
            }
            refCounter = other.refCounter;
            id = other.id;
            instance = other.instance;
            if (refCounter) {
                refCounter->incrementReference(id);
            }
            return *this;
        }

        RenderObjectHandle(RenderObjectHandle &&other) noexcept {
            refCounter = other.refCounter;
            id = other.id;
            instance = std::move(other.instance);
            other.refCounter = nullptr;
        }

        RenderObjectHandle &operator=(RenderObjectHandle &&other) noexcept {
            if (this == &other) {
                return *this;
            }
            refCounter = other.refCounter;
            id = other.id;
            instance = std::move(other.instance);
            other.refCounter = nullptr;
            return *this;
        }

        ~RenderObjectHandle() {
            if (refCounter) {
                refCounter->decrementReference(id);
            }
        }

        [[nodiscard]] bool isAssigned() const {
            return refCounter != nullptr;
        }

        T &get() {
            return instance;
        }

        const T &get() const {
            return instance;
        }

        [[nodiscard]] RenderObject::ID getID() const {
            return id;
        }

        T *operator ->() {
            return &instance;
        }

        const T *operator ->() const {
            return &instance;
        }

    private:
        RenderObjectRefCounter *refCounter = nullptr;
        RenderObject::ID id = RenderObject::UNASSIGNED_ID;
        T instance{};
    };
}

#endif //XENGINE_RENDEROBJECT_HPP
