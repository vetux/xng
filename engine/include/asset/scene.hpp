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

#include <utility>

#include "asset/camera.hpp"
#include "asset/light.hpp"
#include "asset/material.hpp"
#include "asset/skybox.hpp"
#include "asset/shader.hpp"
#include "asset/mesh.hpp"

#include "resource/resourcehandle.hpp"

namespace xng {
    struct XENGINE_EXPORT Scene {
        struct XENGINE_EXPORT Object {
            Object(ResourceHandle<Mesh> mesh, ResourceHandle<Material> material)
                    : mesh(std::move(mesh)), material(std::move(material)) {}

            Transform transform;

            ResourceHandle<Mesh> mesh;
            ResourceHandle<Material> material;

            bool outline = false;
            ColorRGBA outlineColor;
            float outlineScale = 1.1f;

            bool castShadow = false;
            bool receiveShadow = false;
        };

        Camera camera;
        Skybox skybox;
        std::vector<Light> lights;
        std::vector<Object> objects;
    };
}

#endif //XENGINE_SCENE_HPP
