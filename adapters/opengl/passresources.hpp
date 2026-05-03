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

#ifndef XENGINE_PASSRESOURCES_HPP
#define XENGINE_PASSRESOURCES_HPP

#include <utility>

#include "resourcescope.hpp"

namespace xng::opengl {
    struct PassResources {
        PassResources(ResourceScope stack, ResourceScope heap)
            : stack(std::move(stack)), heap(std::move(heap)) {
        }

        [[nodiscard]] BufferGL &getBuffer(const ResourceId &id) const {
            switch (id.getNameSpace()) {
                case ResourceId::HEAP:
                    return heap.getBuffer(id.getHandle());
                default:
                    return stack.getBuffer(id.getHandle());
            }
        }

        [[nodiscard]] TextureGL &getTexture(const ResourceId &id) const {
            switch (id.getNameSpace()) {
                case ResourceId::HEAP:
                    return heap.getTexture(id.getHandle());
                default:
                    return stack.getTexture(id.getHandle());
            }
        }

    private:
        ResourceScope stack;
        ResourceScope heap;
    };
}

#endif //XENGINE_PASSRESOURCES_HPP
