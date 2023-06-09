/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_FRAMEGRAPHSLOT_HPP
#define XENGINE_FRAMEGRAPHSLOT_HPP

namespace xng {
    enum FrameGraphSlot : int {
        // These color and depth textures are created and cleared by the construction pass.
        SLOT_SCREEN_COLOR = 0, // The combined deferred and forward colors, can be written to by post processing passes.
        SLOT_SCREEN_DEPTH, // The final screen depth

        SLOT_DEFERRED_COLOR, // The combined color of the deferred rendered objects, can be written to by post processing passes which require separated transparency and color textures (DOF).
        SLOT_DEFERRED_DEPTH, // The depth of the deferred rendered objects

        SLOT_FORWARD_COLOR, // The combined color of the forward rendered objects (Transparency)
        SLOT_FORWARD_DEPTH, // The depth of the forward rendered objects

        // The GBuffer Textures
        SLOT_GBUFFER_POSITION,          // Texture RGBA32F : World Space Position xyz, w = X
        SLOT_GBUFFER_NORMAL,                // Texture RGBA32F : Vertex or Texture Normal xyz, w = X
        SLOT_GBUFFER_TANGENT,               // Texture RGBA32F : Vertex Tangent xyz, w = X
        SLOT_GBUFFER_ROUGHNESS_METALLIC_AO, // Texture RGBA32F : .x = pbr roughness or phong shininess, .y = pbr metallic, .z = pbr ambient occlusion
        SLOT_GBUFFER_ALBEDO,                // Texture RGBA : The pbr albedo or phong diffuse color value
        SLOT_GBUFFER_AMBIENT,               // Texture RGBA : The phong ambient color value
        SLOT_GBUFFER_SPECULAR,              // Texture RGBA : The phong specular color value
        SLOT_GBUFFER_MODEL_OBJECT,          // Texture RGBA32I : .x = Shading Model ID, .y = Object ID
        SLOT_GBUFFER_DEPTH,                 // Texture DEPTH_STENCIL : The depth value in the x component

        SLOT_SHADOW_MAP_DIRECTIONAL, // A Texture Array with 2d textures containing directional light maps.
        SLOT_SHADOW_MAP_SPOT, // A Texture Array with 2d textures containing spot light maps.
        SLOT_SHADOW_MAP_POINT, // A Texture Array with 3d cubemap textures containing point light maps.

        // User slots are not used by the default passes included with xng and can be used to share data between user created passes.
        SLOT_USER_1,
        SLOT_USER_2,
        SLOT_USER_3,
        SLOT_USER_4,
        SLOT_USER_5,
        SLOT_USER_6,
        SLOT_USER_7,
        SLOT_USER_8,
        SLOT_USER_9,
        SLOT_USER_10,
        SLOT_USER_11,
        SLOT_USER_12,
        SLOT_USER_13,
        SLOT_USER_14,
        SLOT_USER_15,
        SLOT_USER_16,
        SLOT_USER_17,
        SLOT_USER_18,
        SLOT_USER_19,
        SLOT_USER_20,
    };
}

#endif //XENGINE_FRAMEGRAPHSLOT_HPP
