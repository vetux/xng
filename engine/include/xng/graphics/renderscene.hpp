/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/graphics/scene/staticmodel.hpp"
#include "xng/graphics/scene/skinnedmodel.hpp"
#include "xng/graphics/scene/skybox.hpp"
#include "xng/graphics/camera.hpp"

namespace xng {
    struct StaticModelObject {
        Transform transform;
        ResourceHandle<StaticModel> model;

        // Optional user specified materials for the submeshes
        std::map<size_t, Material> materials;

        bool castShadows = false;
        bool receiveShadows = true;
    };

    struct SkinnedModelObject {
        Transform transform;
        ResourceHandle<SkinnedModel> model;

        // Optional user specified materials for the submeshes
        std::map<size_t, Material> materials;

        bool castShadows = false;
        bool receiveShadows = true;

        // Optional dynamic bone transform values
        std::map<std::string, Mat4f> boneTransforms;
    };

    struct PointLightObject {
        Transform transform;
        PointLight light;

        bool operator==(const PointLightObject &other) const {
            return transform == other.transform && light == other.light;
        }

        bool operator!=(const PointLightObject &other) const {
            return !(*this == other);
        }
    };

    struct DirectionalLightObject {
        Transform transform;
        DirectionalLight light;

        bool operator==(const DirectionalLightObject &other) const {
            return transform == other.transform && light == other.light;
        }

        bool operator!=(const DirectionalLightObject &other) const {
            return !(*this == other);
        }
    };

    struct SpotLightObject {
        Transform transform;
        SpotLight light;

        bool operator==(const SpotLightObject &other) const {
            return transform == other.transform && light == other.light;
        }

        bool operator!=(const SpotLightObject &other) const {
            return !(*this == other);
        }
    };

    struct SpriteObject {
        Transform transform;
        ResourceHandle<Sprite> sprite;
        bool textureFiltering = false;
        float zOffset = 0;
    };

    // TODO: Sprite Lighting

    struct RenderScene {
        Transform cameraTransform;
        Camera camera;

        std::vector<StaticModelObject> staticModels;
        std::vector<SkinnedModelObject> skinnedModels;

        std::vector<PointLightObject> pointLights;
        std::vector<DirectionalLightObject> directionalLights;
        std::vector<SpotLightObject> spotLights;

        std::vector<SpriteObject> sprites;

        Skybox skybox;
    };
}

#endif //XENGINE_SCENE_HPP
