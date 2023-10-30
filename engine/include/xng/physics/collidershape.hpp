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

#ifndef XENGINE_COLLIDERSHAPE_HPP
#define XENGINE_COLLIDERSHAPE_HPP

#include "xng/render/mesh.hpp"
#include "xng/io/messageable.hpp"

namespace xng {
    enum ColliderShapeType {
        COLLIDER_2D, // The vertices / indices are treated as 2D points describing a polygon facing in the z axis.
        COLLIDER_3D // The vertices / indices are treated as 3D points which form faces according to the set primitive.
    };

    struct XENGINE_EXPORT ColliderShape : public Messageable {
        ColliderShapeType type = COLLIDER_2D;
        Primitive primitive = TRIANGLES;
        std::vector<Vec3f> vertices;
        std::vector<size_t> indices; // If not empty the indices into vertices in order.

        bool operator==(const ColliderShape &other) const {
            return type == other.type
                   && primitive == other.primitive
                   && vertices == other.vertices
                   && indices == other.indices;
        }

        Messageable &operator<<(const Message &message) override {
            type = (ColliderShapeType) message.getMessage("type", Message((int) COLLIDER_2D)).asInt();
            primitive = (Primitive) message.getMessage("primitive", Message((int) TRIANGLES)).asInt();
            if (message.has("vertices") && message.getMessage("vertices").getType() == Message::LIST) {
                for (auto &vert: message.getMessage("vertices").asList()) {
                    Vec3f vertex;
                    vertex << vert;
                    vertices.emplace_back(vertex);
                }
            }
            if (message.has("indices") && message.getMessage("indices").getType() == Message::LIST) {
                for (auto &index: message.getMessage("indices").asList()) {
                    indices.emplace_back(index.asLong());
                }
            }
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["type"] = (int) type;
            message["primitive"] = (int) primitive;
            auto vec = std::vector<Message>();
            for (auto &vert: vertices) {
                Message msg;
                vert >> msg;
                vec.emplace_back(msg);
            }
            message["vertices"] = vec;
            vec.clear();
            for (const int &index: indices) {
                vec.emplace_back(Message(index));
            }
            message["indices"] = vec;
            return message;
        }
    };
}

#endif //XENGINE_COLLIDERSHAPE_HPP
