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

#ifndef XENGINE_RENDERPIPELINEMATERIAL_HPP
#define XENGINE_RENDERPIPELINEMATERIAL_HPP

#include <cstdint>
#include <variant>

#include "xng/renderer/objects/rendertexture.hpp"

namespace xng {
    /**
     * A pipeline material represents the per-draw data.
     * The format of a material is dynamically defined when instantiating a pipeline.
     */
    class RenderPipelineMaterial {
    public:
        typedef size_t PropertyID;

        typedef size_t TextureID;

        virtual ~RenderPipelineMaterial() = default;

        virtual void update(const std::unordered_map<PropertyID, rg::ShaderPrimitive> &properties,
                            const std::unordered_map<TextureID, RenderObject::ID> &textures) = 0;

        virtual const std::unordered_map<PropertyID, rg::ShaderPrimitiveType> &getProperties() = 0;

        virtual const std::unordered_set<TextureID> &getTextures() = 0;
    };
}

#endif //XENGINE_RENDERPIPELINEMATERIAL_HPP
