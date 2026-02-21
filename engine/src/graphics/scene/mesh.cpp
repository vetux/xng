/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/graphics/scene/mesh.hpp"

#include "xng/graphics/vertexbuilder.hpp"

#include "xng/math/pi.hpp"

namespace xng
{
    Mesh Mesh::normalizedQuad()
    {
        Mesh nQuad = {};
        nQuad.primitive = TRIANGLES;
        nQuad.vertexLayout = ShaderAttributeLayout({
            {"position", ShaderPrimitiveType::vec3()},
            {"uv", ShaderPrimitiveType::vec2()}
        });
        auto vertData = VertexBuilder().addVec3(Vec3f(-1, 1, 0)).addVec2(Vec2f(0, 0)).build();
        nQuad.vertices.insert(nQuad.vertices.end(), vertData.begin(), vertData.end());
        vertData = VertexBuilder().addVec3(Vec3f(1, 1, 0)).addVec2(Vec2f(1, 0)).build();
        nQuad.vertices.insert(nQuad.vertices.end(), vertData.begin(), vertData.end());
        vertData = VertexBuilder().addVec3(Vec3f(1, -1, 0)).addVec2(Vec2f(1, 1)).build();
        nQuad.vertices.insert(nQuad.vertices.end(), vertData.begin(), vertData.end());
        vertData = VertexBuilder().addVec3(Vec3f(-1, 1, 0)).addVec2(Vec2f(0, 0)).build();
        nQuad.vertices.insert(nQuad.vertices.end(), vertData.begin(), vertData.end());
        vertData = VertexBuilder().addVec3(Vec3f(1, -1, 0)).addVec2(Vec2f(1, 1)).build();
        nQuad.vertices.insert(nQuad.vertices.end(), vertData.begin(), vertData.end());
        vertData = VertexBuilder().addVec3(Vec3f(-1, -1, 0)).addVec2(Vec2f(0, 1)).build();
        nQuad.vertices.insert(nQuad.vertices.end(), vertData.begin(), vertData.end());
        return nQuad;
    }

    Mesh Mesh::normalizedCube(int subdivisions)
    {
        Mesh mesh = {};
        mesh.primitive = TRIANGLES;
        mesh.vertexLayout = ShaderAttributeLayout({
            {"position", ShaderPrimitiveType::vec3()},
            {"normal", ShaderPrimitiveType::vec3()},
            {"uv", ShaderPrimitiveType::vec2()}
        });

        const size_t vertexStride = static_cast<size_t>(mesh.vertexLayout.getLayoutSize());
        std::vector<Vec3f> positions;
        std::vector<Vec3f> normals;
        std::vector<Vec2f> uvs;
        std::vector<unsigned int> indices;

        auto addFace = [&](const Vec3f& normal, const Vec3f& tangent, const Vec3f& bitangent, const Vec3f& center)
        {
            unsigned int baseIndex = static_cast<unsigned int>(positions.size());

            // Generate a subdivided grid for this face
            for (int y = 0; y <= subdivisions; ++y)
            {
                for (int x = 0; x <= subdivisions; ++x)
                {
                    float u = static_cast<float>(x) / static_cast<float>(subdivisions);
                    float v = static_cast<float>(y) / static_cast<float>(subdivisions);

                    // Map from [0,1] to [-1,1]
                    float s = u * 2.0f - 1.0f;
                    float t = v * 2.0f - 1.0f;

                    Vec3f pos = center + tangent * s + bitangent * t;
                    positions.push_back(pos);
                    normals.push_back(normal);
                    uvs.push_back(Vec2f(u, v));
                }
            }

            // Generate indices for this face
            for (int y = 0; y < subdivisions; ++y)
            {
                for (int x = 0; x < subdivisions; ++x)
                {
                    unsigned int i0 = baseIndex + y * (subdivisions + 1) + x;
                    unsigned int i1 = i0 + 1;
                    unsigned int i2 = i0 + (subdivisions + 1);
                    unsigned int i3 = i2 + 1;

                    // Two triangles per quad
                    indices.push_back(i0);
                    indices.push_back(i2);
                    indices.push_back(i1);

                    indices.push_back(i1);
                    indices.push_back(i2);
                    indices.push_back(i3);
                }
            }
        };

        // +X face (right)
        addFace(Vec3f(1, 0, 0), Vec3f(0, 0, -1), Vec3f(0, 1, 0), Vec3f(1, 0, 0));
        // -X face (left)
        addFace(Vec3f(-1, 0, 0), Vec3f(0, 0, 1), Vec3f(0, 1, 0), Vec3f(-1, 0, 0));
        // +Y face (top)
        addFace(Vec3f(0, 1, 0), Vec3f(1, 0, 0), Vec3f(0, 0, -1), Vec3f(0, 1, 0));
        // -Y face (bottom)
        addFace(Vec3f(0, -1, 0), Vec3f(1, 0, 0), Vec3f(0, 0, 1), Vec3f(0, -1, 0));
        // +Z face (front)
        addFace(Vec3f(0, 0, 1), Vec3f(1, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, 1));
        // -Z face (back)
        addFace(Vec3f(0, 0, -1), Vec3f(-1, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, -1));

        // Pack vertices into buffer
        for (size_t i = 0; i < positions.size(); ++i)
        {
            auto vertData = VertexBuilder()
                            .addVec3(positions[i])
                            .addVec3(normals[i])
                            .addVec2(uvs[i])
                            .build();
            mesh.vertices.insert(mesh.vertices.end(), vertData.begin(), vertData.end());
        }

        mesh.indices = std::move(indices);
        return mesh;
    }

    Mesh Mesh::sphere(float radius, int latitudes, int longitudes)
    {
        //https://gist.github.com/Pikachuxxxx/5c4c490a7d7679824e0e18af42918efc
        if (longitudes < 3)
            longitudes = 3;
        if (latitudes < 2)
            latitudes = 2;

        std::vector<Vec3f> positions;
        std::vector<Vec3f> normals;
        std::vector<Vec2f> uvs;
        std::vector<unsigned int> indices;

        float nx, ny, nz, lengthInv = 1.0f / radius; // normal
        // Temporary vertex
        struct Vertex
        {
            float x, y, z, s, t; // Postion and Texcoords
        };

        float deltaLatitude = PI / latitudes;
        float deltaLongitude = 2 * PI / longitudes;
        float latitudeAngle;
        float longitudeAngle;

        // Compute all vertices first except normals
        for (int i = 0; i <= latitudes; ++i)
        {
            latitudeAngle = PI / 2 - i * deltaLatitude; /* Starting -pi/2 to pi/2 */
            float xy = radius * cosf(latitudeAngle); /* r * cos(phi) */
            float z = radius * sinf(latitudeAngle); /* r * sin(phi )*/

            /*
             * We add (latitudes + 1) vertices per longitude because of equator,
             * the North pole and South pole are not counted here, as they overlap.
             * The first and last vertices have same position and normal, but
             * different tex coords.
             */
            for (int j = 0; j <= longitudes; ++j)
            {
                longitudeAngle = j * deltaLongitude;

                Vertex vertex{};
                vertex.x = xy * cosf(longitudeAngle); /* x = r * cos(phi) * cos(theta)  */
                vertex.y = xy * sinf(longitudeAngle); /* y = r * cos(phi) * sin(theta) */
                vertex.z = z; /* z = r * sin(phi) */
                vertex.s = (float)j / longitudes; /* s */
                vertex.t = (float)i / latitudes; /* t */
                positions.emplace_back(vertex.x, vertex.y, vertex.z);
                uvs.emplace_back(vertex.s, vertex.t);

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
        for (int i = 0; i < latitudes; ++i)
        {
            k1 = i * (longitudes + 1);
            k2 = k1 + longitudes + 1;
            // 2 Triangles per latitude block excluding the first and last longitudes blocks
            for (int j = 0; j < longitudes; ++j, ++k1, ++k2)
            {
                if (i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                if (i != (latitudes - 1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        Mesh ret;
        ret.primitive = TRIANGLES;
        ret.vertexLayout = ShaderAttributeLayout({
            {"position", ShaderPrimitiveType::vec3()},
            {"normal", ShaderPrimitiveType::vec3()},
            {"uv", ShaderPrimitiveType::vec2()}
        });
        for (auto i = 0; i < positions.size(); ++i)
        {
            auto position = positions.at(i);
            auto norm = normals.at(i);
            auto uv = uvs.at(i);
            auto vertdata = VertexBuilder().addVec3(position).addVec3(norm).addVec2(uv).build();
            ret.vertices.insert(ret.vertices.end(), vertdata.begin(), vertdata.end());
        }
        ret.indices = std::move(indices);
        return ret;
    }
}
