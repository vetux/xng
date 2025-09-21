/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_SCENE_HPP
#define XENGINE_SCENE_HPP

#include "xng/graphics/scene/sprite.hpp"
#include "xng/graphics/scene/pointlight.hpp"
#include "xng/graphics/scene/directionallight.hpp"
#include "xng/graphics/scene/spotlight.hpp"

#include "xng/graphics/scene/mesh.hpp"
#include "xng/graphics/scene/skinnedmesh.hpp"
#include "xng/graphics/scene/skybox.hpp"
#include "xng/graphics/camera.hpp"

namespace xng {
    struct MeshObject {
        Transform transform;
        ResourceHandle<Mesh> mesh;

        // Optional user specified materials for the mesh (materials[0]) and submeshes (materials[1...])
        std::map<size_t, ResourceHandle<Material> > materials;

        bool castShadows = false;
        bool receiveShadows = false;
    };

    struct SkinnedMeshObject {
        Transform transform;
        ResourceHandle<SkinnedMesh> mesh;

        // Optional user specified materials for the mesh (materials[0]) and submeshes (materials[1...])
        std::map<size_t, ResourceHandle<Material> > materials;

        bool castShadows = false;
        bool receiveShadows = false;

        // Optional dynamic bone transform values which override the values in SkinnedMesh.rig, For animation
        std::map<std::string, Mat4f> boneTransforms;
    };

    struct PointLightObject {
        Transform transform;
        PointLight light;
        bool castShadows = false;
    };

    struct DirectionalLightObject {
        Transform transform;
        DirectionalLight light;
        bool castShadows = false;
    };

    struct SpotLightObject {
        Transform transform;
        SpotLight light;
        bool castShadows = false;
    };

    struct SpriteObject {
        Transform transform;
        ResourceHandle<Sprite> sprite;
        bool textureFiltering = false;
        float zOffset = 0;
    };

    // TODO: Sprite Lighting

    struct CanvasObject {
        Transform transform;

        // If false the canvas plane is rendered with an orthographic projection with the transform applied as an offset.
        bool worldSpace = false;

        size_t batchIndex = 0; // The index of the 2d render batch containing the canvas contents.
    };

    /**
     * The runtime scene render data.
     * Designed to be small enough to be rebuilt every frame.
     */
    struct XENGINE_EXPORT RenderScene {
        Transform cameraTransform;
        Camera camera;

        std::vector<MeshObject> meshes;
        std::vector<SkinnedMeshObject> skinnedMeshes;

        std::vector<PointLightObject> pointLights;
        std::vector<DirectionalLightObject> directionalLights;
        std::vector<SpotLightObject> spotLights;

        std::vector<SpriteObject> sprites;

        Skybox skybox;
    };
}

#endif //XENGINE_SCENE_HPP
