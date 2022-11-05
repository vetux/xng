/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "camera.hpp"
#include "light.hpp"
#include "material.hpp"
#include "skybox.hpp"
#include "shader.hpp"
#include "mesh.hpp"

#include "xng/resource/resourcehandle.hpp"

namespace xng {
    struct XENGINE_EXPORT Scene : public Messageable {
        struct XENGINE_EXPORT Object : public Messageable {
            Transform transform;

            ResourceHandle<Mesh> mesh;
            ResourceHandle<Material> material;

            bool castShadows = false;
            bool receiveShadows = false;

            bool outline = false; // If true the object that the material belongs to is redrawn with object.scale * outlineScale scale and all fragments not belonging to the unscaled object are colored with the specified color, and faded alpha values towards the edges beginning at borderWidth * fadeStart.
            ColorRGBA outlineColor;
            float outlineScale = 1.1f;
            float outlineFadeStart = 0.5f;

            Messageable &operator<<(const Message &message) override {
                transform << message.value("transform");
                mesh << message.value("mesh");
                material << message.value("material");
                castShadows = message.value("castShadows", false);
                receiveShadows = message.value("receiveShadows", false);
                outline = message.value("outline", false);
                outlineScale = message.value("outlineScale", 1.1f);
                outlineFadeStart = message.value("outlineFadeStart", 0.5f);
                return *this;
            }

            Message &operator>>(Message &message) const override {
                message = Message(Message::DICTIONARY);
                transform >> message["transform"];
                mesh >> message["mesh"];
                material >> message["material"];
                message["castShadows"] = castShadows;
                message["receiveShadows"] = receiveShadows;
                message["outline"] = outline;
                message["outlineScale"] = outlineScale;
                message["outlineFadeStart"] = outlineFadeStart;
                return message;
            }
        };

        Transform cameraTransform;
        Camera camera;
        Skybox skybox;
        std::vector<Light> lights;
        std::vector<Object> objects;

        Messageable &operator<<(const Message &message) override {
            cameraTransform << message.value("cameraTransform");
            camera << message.value("camera");
            skybox << message.value("skybox");

            if (message.has("lights") && message.value("lights").getType() == Message::LIST) {
                for (auto &v: message.value("lights").asList()) {
                    Light l;
                    l << v;
                    lights.emplace_back(l);
                }
            }

            if (message.has("objects") && message.value("objects").getType() == Message::LIST) {
                for (auto &v: message.value("objects").asList()) {
                    Object o;
                    o << v;
                    objects.emplace_back(o);
                }
            }
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);

            cameraTransform >> message["cameraTransform"];
            camera >> message[""];
            skybox >> message [""];

            std::vector<Message> vec;
            for (auto &light : lights){
                Message msg;
                light >> msg;
                vec.emplace_back(msg);
            }
            message["lights"] = vec;

            vec.clear();
            for (auto &obj : objects){
                Message msg;
                obj >> msg;
                vec.emplace_back(msg);
            }
            message["objects"] = vec;

            return message;
        }
    };
}

#endif //XENGINE_SCENE_HPP
