/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_SPHERECONTROLLERSYSTEM_HPP
#define XENGINE_SPHERECONTROLLERSYSTEM_HPP

#include "xng/xng.hpp"

using namespace xng;

struct SphereControllerComponent : public Component {
    std::type_index getType() const override {
        return typeid(SphereControllerComponent);
    }
};

class SphereControllerSystem : public System {
public:
    Input &input;

    SphereControllerSystem(Input &input) : input(input) {}

    void start(EntityScene &scene, EventBus &eventBus) override {
        System::start(scene, eventBus);
    }

    void stop(EntityScene &scene, EventBus &eventBus) override {
        System::stop(scene, eventBus);
    }

    void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) override {
        Vec3f in;
        if (input.getKey(KEY_KP_4)){
            in.x = -1;
        } else if (input.getKey(KEY_KP_6))  {
            in.x = 1;
        }

        if (input.getKey(KEY_KP_8)){
            in.z = -1;
        } else if (input.getKey(KEY_KP_2))  {
            in.z = 1;
        }

        if (input.getKey(KEY_KP_PLUS)){
            in.y = 1;
        }

        for (auto &pair : scene.getPool<SphereControllerComponent>()){
            if (scene.checkComponent<RigidBodyComponent>(pair.first)){
                auto t = scene.getComponent<RigidBodyComponent>(pair.first);
                t.angularImpulse =( in  *5);
                scene.updateComponent(pair.first, t);
            }
        }
    }

    std::string getName() override {
        return "SphereControllerSystem";
    }

};

#endif //XENGINE_SPHERECONTROLLERSYSTEM_HPP
