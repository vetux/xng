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

#ifndef XENGINE_COLLIDERDEF_HPP
#define XENGINE_COLLIDERDEF_HPP

#include "xng/resource/resource.hpp"
#include "collidershape.hpp"

namespace xng {
    struct ColliderDesc : public Resource, public Messageable {
        ColliderShape shape;
        float friction{};
        float restitution{};
        float restitution_threshold{};
        float density{};
        bool isSensor{};

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<ColliderDesc>(*this);
        }

        std::type_index getTypeIndex() override {
            return typeid(ColliderDesc);
        }

        bool operator==(const ColliderDesc &other) const {
            return shape == other.shape
                   && friction == other.friction
                   && restitution == other.restitution
                   && restitution_threshold == other.restitution_threshold
                   && density == other.density
                   && isSensor == other.isSensor;
        }

        Messageable &operator<<(const Message &message) override {
            shape << message.getMessage("shape");
            message.value("friction", friction, 1.0f);
            message.value("restitution", restitution, 0.0f);
            message.value("restitution_threshold", restitution_threshold, 0.0f);
            message.value("density", density, 1.0f);
            message.value("isSensor", isSensor, false);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            shape >> message["shape"];
            message["friction"] = friction;
            message["restitution"] = restitution;
            message["restitution_threshold"] = restitution_threshold;
            message["density"] = density;
            message["isSensor"] = isSensor;
            return message;
        }
    };
}

#endif //XENGINE_COLLIDERDEF_HPP
