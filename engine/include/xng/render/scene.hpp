/**
 *  This file is part of xEngine, a C++ game engine library.
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

#ifndef XENGINE_SCENE_HPP
#define XENGINE_SCENE_HPP

#include <utility>

#include "xng/asset/camera.hpp"
#include "xng/asset/directionallight.hpp"
#include "xng/asset/pointlight.hpp"
#include "xng/asset/spotlight.hpp"
#include "xng/asset/material.hpp"
#include "xng/asset/skybox.hpp"
#include "xng/asset/shader.hpp"
#include "xng/asset/mesh.hpp"

#include "xng/types/genericmap.hpp"

#include "xng/resource/resourcehandle.hpp"

namespace xng {
    /**
     * The runtime scene render data.
     */
    struct XENGINE_EXPORT Scene {
        struct XENGINE_EXPORT Object {
            Transform transform;

            ResourceHandle<Mesh> mesh;
            ResourceHandle<Material> material;

            bool castShadows = false;
            bool receiveShadows = false;
        };

        Transform cameraTransform;
        Camera camera;

        Skybox skybox;

        std::vector<DirectionalLight> directionalLights;
        std::vector<PointLight> pointLights;
        std::vector<SpotLight> spotLights;

        std::vector<Object> objects;
    };
}

#endif //XENGINE_SCENE_HPP
