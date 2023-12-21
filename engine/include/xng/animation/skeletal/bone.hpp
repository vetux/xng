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

#ifndef XENGINE_BONE_HPP
#define XENGINE_BONE_HPP

#include "xng/math/matrix.hpp"

namespace xng {
    struct VertexWeight : public Messageable {
        size_t vertex{}; // The index of the vertex in Mesh::vertices
        float weight{};

        Messageable &operator<<(const Message &message) override {
            message.value("vertex", vertex);
            message.value("weight", weight);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            vertex >> message["vertex"];
            weight >> message["weight"];
            return message;
        }
    };

    struct Bone : public Messageable {
        std::string name; // The name of the bone
        Mat4f transform;
        Mat4f offset;
        std::vector<VertexWeight> weights;

        Messageable &operator<<(const Message &message) override {
            message.value("name", name);
            message.value("transform", transform);
            message.value("offset", offset);
            message.value("weights", weights);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            name >> message["name"];
            transform >> message["transform"];
            offset >> message["offset"];
            weights >> message["weights"];
            return message;
        }
    };
}

#endif //XENGINE_BONE_HPP
