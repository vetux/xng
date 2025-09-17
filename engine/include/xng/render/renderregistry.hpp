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
#include "scene/scene.hpp"
#include "xng/math/vector2.hpp"
#include "xng/rendergraph/rendergraphresource.hpp"

namespace xng {
    enum RenderRegistryEntry {
        // The combined deferred and forward colors from the compositing pass, can be written to by post processing or overlay (eg. gui) passes.
        SCREEN_COLOR = 0,
        SCREEN_DEPTH, // The combined deferred and forward screen depth from the compositing pass

        // The combined color of the deferred rendered objects, can be written to by post processing passes which require separated transparency and color textures (DOF).
        DEFERRED_COLOR,
        DEFERRED_DEPTH, // The depth of the deferred rendered objects

        FORWARD_COLOR, // The combined color of the forward rendered objects (Transparency)
        FORWARD_DEPTH, // The depth of the forward rendered objects

        // The non-transparent color of the background (eg. Skybox), the compositing pass blits the deferred color over this based on the depth and then blends the forward color ontop and stores the result in screen color.
        BACKGROUND_COLOR,

        // The GBuffer Textures, Created and assigned by the construction pass
        GBUFFER_POSITION, // Texture RGBA32F : World Space Position xyz, w = X
        GBUFFER_NORMAL, // Texture RGBA32F : Vertex or Texture Normal xyz, w = X
        GBUFFER_TANGENT, // Texture RGBA32F : Vertex Tangent xyz, w = X
        GBUFFER_ROUGHNESS_METALLIC_AO, // Texture RGBA32F : .x = roughness, .y = metallic, .z = ambient occlusion
        GBUFFER_ALBEDO, // Texture RGBA : .xyzw = Albedo
        GBUFFER_OBJECT_SHADOWS, // Texture RGBA32I : .x = Object ID, .y = Receive Shadows
        GBUFFER_DEPTH, // Texture DEPTH : The depth value in the x component
    };

    class RenderRegistry {
    public:
        void setEntry(RenderRegistryEntry entry, RenderGraphResource value) {
            entries[entry] = value;
        }

        RenderGraphResource getEntry(RenderRegistryEntry entry) {
            return entries[entry];
        }

    private:
        std::unordered_map<RenderRegistryEntry, RenderGraphResource> entries;
    };
}

#endif //XENGINE_RENDERREGISTRY_HPP
