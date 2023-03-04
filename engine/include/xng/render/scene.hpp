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
#include "xng/asset/light.hpp"
#include "xng/asset/material.hpp"
#include "xng/asset/skybox.hpp"
#include "xng/asset/shader.hpp"
#include "xng/asset/mesh.hpp"

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

            bool outline = false; // If true the object is redrawn with object.scale * outlineScale scale and all fragments not belonging to the unscaled object are colored with the specified color, and faded alpha values towards the edges beginning at borderWidth * fadeStart.
            ColorRGBA outlineColor;
            float outlineScale = 1.1f;
            float outlineFadeStart = 0.5f;
        };

        Transform cameraTransform;
        Camera camera;
        Skybox skybox;
        std::vector<Light> lights;
        std::vector<Object> objects;
    };
}

#endif //XENGINE_SCENE_HPP
