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

#ifndef XENGINE_LIGHTCOMPONENT_HPP
#define XENGINE_LIGHTCOMPONENT_HPP

#include "xng/asset/directionallight.hpp"
#include "xng/asset/spotlight.hpp"
#include "xng/asset/pointlight.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    struct XENGINE_EXPORT LightComponent : public Component {
        enum LightType : int {
            LIGHT_DIRECTIONAL = 0,
            LIGHT_POINT,
            LIGHT_SPOT
        } type;

        std::variant<DirectionalLight, PointLight, SpotLight> light;

        Messageable &operator<<(const Message &message) override {
            message.value("type", reinterpret_cast<int&>(type), static_cast<int>(LIGHT_DIRECTIONAL));
            switch (type) {
                case LIGHT_DIRECTIONAL: {
                    DirectionalLight tmp;
                    message.value("light", tmp);
                    light = tmp;
                    break;
                }
                case LIGHT_POINT: {
                    PointLight tmp;
                    message.value("light", tmp);
                    light = tmp;
                    break;
                }
                case LIGHT_SPOT: {
                    SpotLight tmp;
                    message.value("light", tmp);
                    light = tmp;
                    break;
                }
            }
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            type >> message["type"];
            switch (type) {
                case LIGHT_DIRECTIONAL: {
                    std::get<DirectionalLight>(light) >> message["light"];
                    break;
                }
                case LIGHT_POINT: {
                    std::get<PointLight>(light) >> message["light"];
                    break;
                }
                case LIGHT_SPOT: {
                    std::get<SpotLight>(light) >> message["light"];
                    break;
                }
            }
            return Component::operator>>(message);
        }

        std::type_index getType() const override {
            return typeid(LightComponent);
        }
    };
}

#endif //XENGINE_LIGHTCOMPONENT_HPP
