/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "asset/mesh.hpp"

#include <string>
#include <sstream>

#include "resource/resourceimporter.hpp"

#include "resource/staticresource.hpp"

static const std::string NORM_CUBE_OBJ = std::string(R"###(
o Cube
v 1.000000 1.000000 -1.000000
v 1.000000 -1.000000 -1.000000
v 1.000000 1.000000 1.000000
v 1.000000 -1.000000 1.000000
v -1.000000 1.000000 -1.000000
v -1.000000 -1.000000 -1.000000
v -1.000000 1.000000 1.000000
v -1.000000 -1.000000 1.000000
vt 0.000000 1.000000
vt 1.000000 0.000000
vt 1.000000 1.000000
vt 1.000000 1.000000
vt 0.000000 0.000000
vt 1.000000 0.000000
vt 0.000000 1.000000
vt 1.000000 0.000000
vt 1.000000 1.000000
vt 0.000000 1.000000
vt 0.000000 0.000000
vt 1.000000 0.000000
vt 0.000000 0.000000
vt 0.000000 0.000000
vt 1.000000 1.000000
vt 0.000000 1.000000
vn 0.0000 1.0000 0.0000
vn 0.0000 0.0000 1.0000
vn -1.0000 0.0000 0.0000
vn 0.0000 -1.0000 0.0000
vn 1.0000 0.0000 0.0000
vn 0.0000 0.0000 -1.0000
s off
f 5/1/1 3/2/1 1/3/1
f 3/4/2 8/5/2 4/6/2
f 7/7/3 6/8/3 8/5/3
f 2/9/4 8/5/4 6/10/4
f 1/3/5 4/11/5 2/12/5
f 5/1/6 2/12/6 6/13/6
f 5/1/1 7/14/1 3/2/1
f 3/4/2 7/7/2 8/5/2
f 7/7/3 5/15/3 6/8/3
f 2/9/4 4/6/4 8/5/4
f 1/3/5 3/16/5 4/11/5
f 5/1/6 1/3/6 2/12/6
)###");

namespace xng {
    static bool nQuadC = false;
    static Mesh nQuad;

    static bool nCubeC = false;
    static Mesh nCube;

    const xng::Mesh &xng::Mesh::normalizedQuad() {
        if (!nQuadC) {
            nQuadC = true;
            nQuad = {TRI,
                     {
                             Vertex({-1, 1, 0}, {0, 1}),
                             Vertex({1, 1, 0}, {1, 1}),
                             Vertex({1, -1, 0}, {1, 0}),
                             Vertex({-1, 1, 0}, {0, 1}),
                             Vertex({1, -1, 0}, {1, 0}),
                             Vertex({-1, -1, 0}, {0, 0})
                     }};
        }
        return nQuad;
    }

    const Mesh &Mesh::normalizedCube() {
        if (!nCubeC) {
            nCubeC = true;
            std::stringstream stream(NORM_CUBE_OBJ);
            nCube = ResourceImporter().import(stream, ".obj").get<Mesh>();
        }
        return nCube;
    }

    std::type_index Mesh::getTypeIndex() {
        return typeid(Mesh);
    }
}