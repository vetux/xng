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

#ifndef XENGINE_COLLIDERSHAPE_HPP
#define XENGINE_COLLIDERSHAPE_HPP

#include "xng/asset/mesh.hpp"
#include "xng/io/messageable.hpp"

namespace xng {
    enum ColliderShapeType {
        COLLIDER_2D, // The vertices / indices are treated as 2D points describing a polygon facing in the z axis.
        COLLIDER_3D // The vertices / indices are interpreted according to the set primitive.
    };

    struct XENGINE_EXPORT ColliderShape : public Messageable {
        ColliderShapeType type = COLLIDER_3D;
        Primitive primitive = TRI;
        std::vector<Vec3f> vertices;
        std::vector<size_t> indices; // If not empty the indices into vertices in order.

        bool operator==(const ColliderShape &other) const {
            return type == other.type
                   && primitive == other.primitive
                   && vertices == other.vertices
                   && indices == other.indices;
        }

        Primitive getPrimitive(const std::string &str) const {
            static const std::map<Primitive, std::string> primNames = std::map<Primitive, std::string>(
                    {
                            {Primitive::POINT, "point"},
                            {Primitive::LINE,  "line"},
                            {Primitive::TRI,   "tri"},
                            {Primitive::QUAD,  "quad"},
                    });
            static const std::map<std::string, Primitive> namePrims = std::map<std::string, Primitive>(
                    {
                            {primNames.at(POINT), POINT},
                            {primNames.at(LINE),  LINE},
                            {primNames.at(TRI),   TRI},
                            {primNames.at(QUAD),  QUAD},
                    });
            return namePrims.at(str);
        }

        std::string getPrimitive(Primitive prim) const {
            static const std::map<Primitive, std::string> primNames = std::map<Primitive, std::string>(
                    {
                            {Primitive::POINT, "point"},
                            {Primitive::LINE,  "line"},
                            {Primitive::TRI,   "tri"},
                            {Primitive::QUAD,  "quad"},
                    });
            return primNames.at(prim);
        }

        Messageable &operator<<(const Message &message) override {
            type = (ColliderShapeType) message.value("type", (int) COLLIDER_3D);
            primitive = getPrimitive(message.value("primitive", std::string("tri")));
            auto vec = message.value("vertices");
            if (vec.getType() == Message::LIST) {
                for (auto &vert: vec.asList()) {
                    Vec3f vertex;
                    vertex << vert;
                    vertices.emplace_back(vertex);
                }
            }
            vec = message.value("indices");
            if (vec.getType() == Message::LIST) {
                for (auto &index: vec.asList()) {
                    indices.emplace_back(index.asLong());
                }
            }
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["type"] = (int) type;
            message["primitive"] = getPrimitive(primitive);
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

        static ColliderShape fromMesh(const Mesh &mesh) {
            ColliderShape ret;
            ret.primitive = mesh.primitive;
            for (auto &v: mesh.vertices) {
                ret.vertices.emplace_back(v.position());
            }
            for (auto &v: mesh.indices) {
                ret.indices.emplace_back(v);
            }
            return ret;
        }
    };
}

#endif //XENGINE_COLLIDERSHAPE_HPP
