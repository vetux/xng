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

#ifndef XENGINE_TRANSIENTSTACK_HPP
#define XENGINE_TRANSIENTSTACK_HPP

#include <memory>
#include <unordered_map>

#include "xng/rendergraph/resourceid.hpp"

#include "resource/buffergl.hpp"
#include "resource/texturegl.hpp"

namespace xng::opengl {
    struct ResourceScope {
        [[nodiscard]] BufferGL &getBuffer(rendergraph::ResourceId::Handle id) const {
            return *buffers.at(id);
        }

        [[nodiscard]] TextureGL &getTexture(rendergraph::ResourceId::Handle id) const {
            return *textures.at(id);
        }

        std::unordered_map<rendergraph::ResourceId::Handle, std::shared_ptr<BufferGL>> buffers{};
        std::unordered_map<rendergraph::ResourceId::Handle, std::shared_ptr<TextureGL>> textures{};
    };
}

#endif //XENGINE_TRANSIENTSTACK_HPP
