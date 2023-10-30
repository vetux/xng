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

#ifndef XENGINE_LIGHTCOMPONENT_HPP
#define XENGINE_LIGHTCOMPONENT_HPP

#include "xng/render/phong/phongdirectionallight.hpp"
#include "xng/render/phong/phongspotlight.hpp"
#include "xng/render/phong/phongpointlight.hpp"
#include "xng/render/pbr/pbrpointlight.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    struct XENGINE_EXPORT LightComponent : public Component {
        std::variant<PhongDirectionalLight,
                     PhongPointLight,
                     PhongSpotLight,
                     PBRPointLight> light;

        Messageable &operator<<(const Message &message) override {
            int type;
            message.value("type", type, 0);
            switch (type) {
                case 0: {
                    PhongDirectionalLight tmp;
                    message.value("light", tmp);
                    light = tmp;
                    break;
                }
                case 1: {
                    PhongPointLight tmp;
                    message.value("light", tmp);
                    light = tmp;
                    break;
                }
                case 2: {
                    PhongSpotLight tmp;
                    message.value("light", tmp);
                    light = tmp;
                    break;
                }
                case 3: {
                    PBRPointLight tmp;
                    message.value("light", tmp);
                    light = tmp;
                }
            }
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            light.index() >> message["type"];
            switch (light.index()) {
                case 0: {
                    std::get<PhongDirectionalLight>(light) >> message["light"];
                    break;
                }
                case 1: {
                    std::get<PhongPointLight>(light) >> message["light"];
                    break;
                }
                case 2: {
                    std::get<PhongSpotLight>(light) >> message["light"];
                    break;
                }
                case 3: {
                    std::get<PBRPointLight>(light) >> message["light"];
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
