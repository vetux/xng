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

namespace xengine {
    static bool screenQuadC = false;
    static Mesh screenQuadMesh;

    const xengine::Mesh &xengine::Mesh::screenQuad() {
        if (!screenQuadC) {
            screenQuadC = true;
            screenQuadMesh = {Mesh::TRI,
                           {
                                   Vertex({-1, 1, 0}, {0, 1}),
                                   Vertex({1, 1, 0}, {1, 1}),
                                   Vertex({1, -1, 0}, {1, 0}),
                                   Vertex({-1, 1, 0}, {0, 1}),
                                   Vertex({1, -1, 0}, {1, 0}),
                                   Vertex({-1, -1, 0}, {0, 0})
                           }};
        }
        return screenQuadMesh;
    }
}