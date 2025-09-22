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

#ifndef XENGINE_NODETEXTURE_HPP
#define XENGINE_NODETEXTURE_HPP

#include <string>

#include "xng/rendergraph/shader/shadernode.hpp"

namespace xng {
    struct NodeTexture final : ShaderNode {
        std::string textureName;

        // An optional index into ShaderSource.textureArrays.at(textureName),
        // if no index is specified, the texture at ShaderSource.textureArrays.at(textureName)[0] is accessed.
        std::unique_ptr<ShaderNode> textureIndex;

        explicit NodeTexture( std::string textureName, std::unique_ptr<ShaderNode> textureIndex)
            : textureName(std::move(textureName)), textureIndex(std::move(textureIndex)) {
        }

        NodeType getType() const override {
            return TEXTURE;
        }

        std::unique_ptr<ShaderNode> copy() const override {
            return std::make_unique<NodeTexture>(textureName, textureIndex ? textureIndex->copy() : nullptr);
        }
    };
}

#endif //XENGINE_NODETEXTURE_HPP
