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

#include "xng/render/scene/mesh.hpp"
#include "xng/io/messageable.hpp"

namespace xng {
    enum ColliderShapeType {
        COLLIDER_2D, // The vertices are treated as 2D points describing a polygon facing in the z axis.
        COLLIDER_SPHERE, // A sphere collider of the specified radius
        COLLIDER_BOX, // A box collider with half extent
        COLLIDER_CYLINDER, // A cylinder collider with radius and height
        COLLIDER_CAPSULE, // A capsule collider with radius and height
        COLLIDER_CONE, // A cone collider with radius and height
        COLLIDER_CONVEX_HULL, // The vertices / indices are treated as 3d points describing a convex hull
        COLLIDER_TRIANGLES, // The vertices / indices are treated as 3d triangles, only supported for static bodies
    };

    /**
     * A description of a shape for a physics simulation
     */
    struct XENGINE_EXPORT ColliderShape :  public Messageable {
        ColliderShapeType type = COLLIDER_2D;

        std::vector<Vec3f> vertices;
        std::vector<unsigned int> indices;

        Vec3f halfExtent;
        float radius;
        float height;

        Messageable &operator<<(const Message &message) override {
            message.value("type", reinterpret_cast<int&>(type));
            message.value("vertices", vertices);
            message.value("indices", indices);
            message.value("halfExtent", halfExtent);
            message.value("radius", radius);
            message.value("height", height);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            type >> message["type"];
            vertices >> message["vertices"];
            indices >> message["indices"];
            halfExtent >> message["halfExtent"];
            radius >> message["radius"];
            height >> message["height"];
            return message;
        }
    };
}

#endif //XENGINE_COLLIDERSHAPE_HPP
