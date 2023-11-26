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

#include "xng/render/mesh.hpp"

#include <string>
#include <sstream>

#include "xng/resource/resourceimporter.hpp"

#include "resource/staticresource.hpp"

#include "xng/render/geometry/vertexbuilder.hpp"

static const std::string NORM_CUBE_OBJ = std::string(R"###(
o Cube
v -1.000000 -1.000000 1.000000
v -1.000000 1.000000 1.000000
v -1.000000 -1.000000 -1.000000
v -1.000000 1.000000 -1.000000
v 1.000000 -1.000000 1.000000
v 1.000000 1.000000 1.000000
v 1.000000 -1.000000 -1.000000
v 1.000000 1.000000 -1.000000
vt 0.375000 0.000000
vt 0.625000 0.000000
vt 0.625000 0.250000
vt 0.375000 0.250000
vt 0.625000 0.500000
vt 0.375000 0.500000
vt 0.625000 0.750000
vt 0.375000 0.750000
vt 0.625000 1.000000
vt 0.375000 1.000000
vt 0.125000 0.500000
vt 0.125000 0.750000
vt 0.875000 0.500000
vt 0.875000 0.750000
vn -1.0000 0.0000 0.0000
vn 0.0000 0.0000 -1.0000
vn 1.0000 0.0000 0.0000
vn 0.0000 0.0000 1.0000
vn 0.0000 -1.0000 0.0000
vn 0.0000 1.0000 0.0000
s off
f 1/1/1 2/2/1 4/3/1 3/4/1
f 3/4/2 4/3/2 8/5/2 7/6/2
f 7/6/3 8/5/3 6/7/3 5/8/3
f 5/8/4 6/7/4 2/9/4 1/10/4
f 3/11/5 7/6/5 5/8/5 1/12/5
f 8/5/6 4/13/6 2/14/6 6/7/6
)###");

namespace xng {
    static bool nQuadC = false;
    static Mesh nQuad;

    static bool nCubeC = false;
    static Mesh nCube;

    const xng::Mesh &xng::Mesh::normalizedQuad() {
        if (!nQuadC) {
            nQuadC = true;
            nQuad = {TRIANGLES,
                     {
                             VertexBuilder().addVec3(Vec3f(-1, 1, 0)).addVec2(Vec2f(0, 1)).build(),
                             VertexBuilder().addVec3(Vec3f(1, 1, 0)).addVec2(Vec2f(1, 1)).build(),
                             VertexBuilder().addVec3(Vec3f(1, -1, 0)).addVec2(Vec2f(1, 0)).build(),
                             VertexBuilder().addVec3(Vec3f(-1, 1, 0)).addVec2(Vec2f(0, 1)).build(),
                             VertexBuilder().addVec3(Vec3f(1, -1, 0)).addVec2(Vec2f(1, 0)).build(),
                             VertexBuilder().addVec3(Vec3f(-1, -1, 0)).addVec2(Vec2f(0, 0)).build(),
                     }};
            nQuad.vertexLayout = VertexLayout({VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                                               VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT)});
        }
        return nQuad;
    }

    const Mesh &Mesh::normalizedCube() {
        if (!nCubeC) {
            nCubeC = true;
            std::stringstream stream(NORM_CUBE_OBJ);
            nCube = *ResourceRegistry::getDefaultRegistry().getImporter(".obj").read(stream, ".obj", "", nullptr).getAll<Mesh>().at(0);
        }
        return nCube;
    }

    std::type_index Mesh::getTypeIndex() const {
        return typeid(Mesh);
    }
}