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

#ifndef XENGINE_FRAMEGRAPH2_HPP
#define XENGINE_FRAMEGRAPH2_HPP

#include <vector>
#include <functional>

#include "xng/render/graph2/fgresource.hpp"

#include "xng/render/graph2/fgpass.hpp"

#include "xng/render/graph2/texture/fgtexture.hpp"
#include "xng/render/graph2/shader/fgshadersource.hpp"

namespace xng {
    class FGContext;

    /**
     * A graph represents an encapsulated rendering operation.
     *
     * The only resource shareable between graphs is the screen texture.
     */
    struct FGGraph {
        std::vector<FGPass> passes;

        std::unordered_map<FGResource, size_t> vertexBufferAllocation;
        std::unordered_map<FGResource, size_t> indexBufferAllocation;
        std::unordered_map<FGResource, size_t> shaderBufferAllocation;

        std::unordered_map<FGResource, FGTexture> textureAllocation;

        std::unordered_map<FGResource, FGShaderSource> shaderAllocation;

        std::unordered_map<FGResource, FGResource> inheritedResources;

        FGResource screenTexture{};

        FGGraph() = default;

        FGGraph(const std::vector<FGPass> &passes,
                const std::unordered_map<FGResource, size_t> &vertex_buffer_allocation,
                const std::unordered_map<FGResource, size_t> &index_buffer_allocation,
                const std::unordered_map<FGResource, size_t> &shader_buffer_allocation,
                const std::unordered_map<FGResource, FGTexture> &texture_allocation,
                const std::unordered_map<FGResource, FGShaderSource> &shader_allocation,
                const std::unordered_map<FGResource, FGResource> &inherited_resources,
                const FGResource screen_texture)
            : passes(passes),
              vertexBufferAllocation(vertex_buffer_allocation),
              indexBufferAllocation(index_buffer_allocation),
              shaderBufferAllocation(shader_buffer_allocation),
              textureAllocation(texture_allocation),
              shaderAllocation(shader_allocation),
              inheritedResources(inherited_resources),
              screenTexture(screen_texture) {
        }
    };
}

#endif //XENGINE_FRAMEGRAPH2_HPP
