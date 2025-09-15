/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include "xng/render/scene/mesh.hpp"

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
            nQuad = {
                TRIANGLES,
                {
                    VertexBuilder().addVec3(Vec3f(-1, 1, 0)).addVec2(Vec2f(0, 1)).build(),
                    VertexBuilder().addVec3(Vec3f(1, 1, 0)).addVec2(Vec2f(1, 1)).build(),
                    VertexBuilder().addVec3(Vec3f(1, -1, 0)).addVec2(Vec2f(1, 0)).build(),
                    VertexBuilder().addVec3(Vec3f(-1, 1, 0)).addVec2(Vec2f(0, 1)).build(),
                    VertexBuilder().addVec3(Vec3f(1, -1, 0)).addVec2(Vec2f(1, 0)).build(),
                    VertexBuilder().addVec3(Vec3f(-1, -1, 0)).addVec2(Vec2f(0, 0)).build(),
                }
            };
            nQuad.vertexLayout = ShaderAttributeLayout({
                {"position", ShaderDataType::vec3()},
                {"uv" ,ShaderDataType::vec2()}
            });
        }
        return nQuad;
    }

    const Mesh &Mesh::normalizedCube() {
        if (!nCubeC) {
            nCubeC = true;
            std::stringstream stream(NORM_CUBE_OBJ);
            nCube = *ResourceRegistry::getDefaultRegistry().getImporter(".obj").read(stream, ".obj", "",
                nullptr).getAll<Mesh>().at(0);
        }
        return nCube;
    }

    Mesh Mesh::sphere(float radius, int latitudes, int longitudes) {
        //https://gist.github.com/Pikachuxxxx/5c4c490a7d7679824e0e18af42918efc

        if (longitudes < 3)
            longitudes = 3;
        if (latitudes < 2)
            latitudes = 2;

        std::vector<Vec3f> vertices;
        std::vector<Vec3f> normals;
        std::vector<Vec2f> uv;
        std::vector<unsigned int> indices;

        float nx, ny, nz, lengthInv = 1.0f / radius; // normal
        // Temporary vertex
        struct Vertex {
            float x, y, z, s, t; // Postion and Texcoords
        };

        float deltaLatitude = M_PI / latitudes;
        float deltaLongitude = 2 * M_PI / longitudes;
        float latitudeAngle;
        float longitudeAngle;

        // Compute all vertices first except normals
        for (int i = 0; i <= latitudes; ++i) {
            latitudeAngle = M_PI / 2 - i * deltaLatitude; /* Starting -pi/2 to pi/2 */
            float xy = radius * cosf(latitudeAngle); /* r * cos(phi) */
            float z = radius * sinf(latitudeAngle); /* r * sin(phi )*/

            /*
             * We add (latitudes + 1) vertices per longitude because of equator,
             * the North pole and South pole are not counted here, as they overlap.
             * The first and last vertices have same position and normal, but
             * different tex coords.
             */
            for (int j = 0; j <= longitudes; ++j) {
                longitudeAngle = j * deltaLongitude;

                Vertex vertex{};
                vertex.x = xy * cosf(longitudeAngle); /* x = r * cos(phi) * cos(theta)  */
                vertex.y = xy * sinf(longitudeAngle); /* y = r * cos(phi) * sin(theta) */
                vertex.z = z; /* z = r * sin(phi) */
                vertex.s = (float) j / longitudes; /* s */
                vertex.t = (float) i / latitudes; /* t */
                vertices.emplace_back(vertex.x, vertex.y, vertex.z);
                uv.emplace_back(vertex.s, vertex.t);

                // normalized vertex normal
                nx = vertex.x * lengthInv;
                ny = vertex.y * lengthInv;
                nz = vertex.z * lengthInv;
                normals.emplace_back(nx, ny, nz);
            }
        }

        /*
         *  Indices
         *  k1--k1+1
         *  |  / |
         *  | /  |
         *  k2--k2+1
         */
        unsigned int k1, k2;
        for (int i = 0; i < latitudes; ++i) {
            k1 = i * (longitudes + 1);
            k2 = k1 + longitudes + 1;
            // 2 Triangles per latitude block excluding the first and last longitudes blocks
            for (int j = 0; j < longitudes; ++j, ++k1, ++k2) {
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                if (i != (latitudes - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        throw std::runtime_error("Not Implemented");
    }

    std::type_index Mesh::getTypeIndex() const {
        return typeid(Mesh);
    }
}
