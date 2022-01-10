/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_SCENE_HPP
#define XENGINE_SCENE_HPP

#include "asset/camera.hpp"
#include "asset/light.hpp"
#include "asset/material.hpp"
#include "asset/skybox.hpp"
#include "asset/shader.hpp"
#include "asset/assethandle.hpp"

#include "platform/graphics/rendercommand.hpp"

namespace xengine {
    /**
     * The collected render scene data.
     */
    struct XENGINE_EXPORT Scene {
        // A deferred draw node description with a mesh, material and drawing parameters
        struct XENGINE_EXPORT DeferredDrawNode {
            DeferredDrawNode() = default;

            DeferredDrawNode(Transform t, AssetHandle<Mesh> mesh, AssetHandle<Material> material)
                    : transform(t), mesh(std::move(mesh)), material(std::move(material)) {}

            Transform transform;
            AssetHandle<Mesh> mesh;
            AssetHandle<Material> material;

            bool outline = false;
            ColorRGBA outlineColor;
            float outlineScale = 1.1f;
        };

        // A forward draw node description with mesh, shaders and textures
        struct XENGINE_EXPORT ForwardDrawNode {
            Transform transform;
            AssetHandle<Mesh> mesh;
            AssetHandle<Shader> shader;
            std::vector<AssetHandle<Texture>> textures;
            RenderProperties properties;
        };

        Camera camera;

        std::vector<Light> lights;

        std::vector<DeferredDrawNode> deferred;
        std::vector<ForwardDrawNode> forward;

        Skybox skybox;
    };
}

#endif //XENGINE_SCENE_HPP
