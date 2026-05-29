/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "xng/assets/mesh.hpp"

#include "xng/math/pi.hpp"

using namespace xng::rg;

namespace xng {
    Mesh Mesh::normalizedQuad() {
        Mesh nQuad = {};

        nQuad.primitive = TRIANGLES;
        nQuad.positions.emplace_back(Vec3f(-1, 1, 0));
        nQuad.uvs.emplace_back(Vec2f(0, 0));
        nQuad.positions.emplace_back(Vec3f(1, 1, 0));
        nQuad.uvs.emplace_back(Vec2f(1, 0));
        nQuad.positions.emplace_back(Vec3f(1, -1, 0));
        nQuad.uvs.emplace_back(Vec2f(1, 1));

        nQuad.positions.emplace_back(Vec3f(-1, 1, 0));
        nQuad.uvs.emplace_back(Vec2f(0, 0));
        nQuad.positions.emplace_back(Vec3f(1, -1, 0));
        nQuad.uvs.emplace_back(Vec2f(1, 1));
        nQuad.positions.emplace_back(Vec3f(-1, -1, 0));
        nQuad.uvs.emplace_back(Vec2f(0, 1));

        return nQuad;
    }

    Mesh Mesh::normalizedCube(int subdivisions) {
        std::vector<Vec3f> positions;
        std::vector<Vec3f> normals;
        std::vector<Vec2f> uvs;
        std::vector<unsigned int> indices;

        auto addFace = [&](const Vec3f &normal, const Vec3f &tangent, const Vec3f &bitangent, const Vec3f &center) {
            unsigned int baseIndex = static_cast<unsigned int>(positions.size());

            // Generate a subdivided grid for this face
            for (int y = 0; y <= subdivisions; ++y) {
                for (int x = 0; x <= subdivisions; ++x) {
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
            for (int y = 0; y < subdivisions; ++y) {
                for (int x = 0; x < subdivisions; ++x) {
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

        Mesh mesh = {};
        mesh.primitive = TRIANGLES;
        mesh.positions = std::move(positions);
        mesh.normals = std::move(normals);
        mesh.uvs = std::move(uvs);
        mesh.indices = std::move(indices);
        return mesh;
    }

    Mesh Mesh::sphere(float radius, int latitudes, int longitudes) {
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
        struct Vertex {
            float x, y, z, s, t; // Postion and Texcoords
        };

        float deltaLatitude = PI / latitudes;
        float deltaLongitude = 2 * PI / longitudes;
        float latitudeAngle;
        float longitudeAngle;

        // Compute all vertices first except normals
        for (int i = 0; i <= latitudes; ++i) {
            latitudeAngle = PI / 2 - i * deltaLatitude; /* Starting -pi/2 to pi/2 */
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

        Mesh ret;
        ret.primitive = TRIANGLES;
        ret.positions = std::move(positions);
        ret.normals = std::move(normals);
        ret.uvs = std::move(uvs);
        ret.indices = std::move(indices);
        return ret;
    }

    static Mesh computeTangents(const Mesh &mesh) {
        assert(!mesh.normals.empty());
        assert(!mesh.uvs.empty());

        const auto vertexCount = mesh.positions.size();

        std::map<std::tuple<float, float, float>, std::vector<unsigned int> > positionToVertices;
        for (unsigned int i = 0; i < vertexCount; i++) {
            const auto &p = mesh.positions.at(i);
            positionToVertices[{p.x, p.y, p.z}].push_back(i);
        }

        std::vector<Vec3f> tangents(vertexCount, Vec3f(0, 0, 0));
        std::vector<Vec3f> bitangents(vertexCount, Vec3f(0, 0, 0));

        auto accumulate = [&](unsigned int i0, unsigned int i1, unsigned int i2) {
            const Vec3f &p0 = mesh.positions.at(i0);
            const Vec3f &p1 = mesh.positions.at(i1);
            const Vec3f &p2 = mesh.positions.at(i2);

            const Vec2f &uv0 = mesh.uvs.at(i0);
            const Vec2f &uv1 = mesh.uvs.at(i1);
            const Vec2f &uv2 = mesh.uvs.at(i2);

            Vec3f edge1 = p1 - p0;
            Vec3f edge2 = p2 - p0;

            float du1 = uv1.x - uv0.x;
            float dv1 = uv1.y - uv0.y;
            float du2 = uv2.x - uv0.x;
            float dv2 = uv2.y - uv0.y;

            float denom = du1 * dv2 - du2 * dv1;
            if (std::abs(denom) < 1e-6f) return;
            float r = 1.0f / denom;

            Vec3f t = (edge1 * dv2 - edge2 * dv1) * r;
            Vec3f b = (edge2 * du1 - edge1 * du2) * r;

            for (auto idx: {i0, i1, i2}) {
                const auto &p = mesh.positions.at(idx);
                for (auto sharedIdx: positionToVertices.at({p.x, p.y, p.z})) {
                    tangents[sharedIdx] += t;
                    bitangents[sharedIdx] += b;
                }
            }
        };

        if (!mesh.indices.empty()) {
            for (size_t i = 0; i < mesh.indices.size(); i += 3)
                accumulate(mesh.indices.at(i), mesh.indices.at(i + 1), mesh.indices.at(i + 2));
        } else {
            for (size_t i = 0; i < vertexCount; i += 3)
                accumulate(i, i + 1, i + 2);
        }

        // Gram-Schmidt orthogonalize against the smooth normal
        for (unsigned int i = 0; i < vertexCount; i++) {
            const Vec3f &n = mesh.normals.at(i);
            Vec3f t = tangents.at(i);

            // Orthogonalize
            t = t - n * n.dot(t);
            float len = t.length();
            if (len > 1e-6f)
                t /= len;
            else {
                Vec3f fallback = std::abs(n.x) < 0.9f ? Vec3f(1, 0, 0) : Vec3f(0, 1, 0);
                fallback = fallback - n * n.dot(fallback);
                t = fallback / fallback.length();
            }

            tangents.at(i) = t;

            Vec3f b = bitangents.at(i);
            b = b - n * n.dot(b) - t * t.dot(b);
            len = b.length();
            if (len > 1e-6f)
                b /= len;
            else
                b = n.cross(t);

            bitangents.at(i) = b;
        }

        Mesh result = mesh;
        result.tangents = std::move(tangents);
        result.bitangents = std::move(bitangents);
        return result;
    }

    Mesh Mesh::computeSmoothNormals(const Mesh &mesh) {
        const auto vertexCount = mesh.positions.size();

        std::map<std::tuple<float, float, float>, std::vector<unsigned int> > positionToVertices;
        for (unsigned int i = 0; i < vertexCount; i++) {
            const auto &p = mesh.positions.at(i);
            positionToVertices[{p.x, p.y, p.z}].push_back(i);
        }

        std::vector<Vec3f> smoothNormals(vertexCount, Vec3f(0, 0, 0));

        auto accumulate = [&](unsigned int i0, unsigned int i1, unsigned int i2) {
            const Vec3f &p0 = mesh.positions.at(i0);
            const Vec3f &p1 = mesh.positions.at(i1);
            const Vec3f &p2 = mesh.positions.at(i2);
            Vec3f faceNormal = (p2 - p0).cross(p1 - p0);

            for (auto idx: {i0, i1, i2}) {
                const auto &p = mesh.positions.at(idx);
                for (auto sharedIdx: positionToVertices.at({p.x, p.y, p.z})) {
                    smoothNormals[sharedIdx] += faceNormal;
                }
            }
        };

        if (!mesh.indices.empty()) {
            for (size_t i = 0; i < mesh.indices.size(); i += 3)
                accumulate(mesh.indices.at(i), mesh.indices.at(i + 1), mesh.indices.at(i + 2));
        } else {
            for (size_t i = 0; i < vertexCount; i += 3)
                accumulate(i, i + 1, i + 2);
        }

        for (auto &n: smoothNormals) {
            float len = n.length();
            if (len > 1e-6f)
                n /= len;
            else
                n = Vec3f(0, 0, 1);
        }

        Mesh result = mesh;
        result.normals = std::move(smoothNormals);
        return computeTangents(result);
    }
}
