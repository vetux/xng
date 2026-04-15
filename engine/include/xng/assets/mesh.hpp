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

#ifndef XENGINE_MESH_HPP
#define XENGINE_MESH_HPP

#include <utility>
#include <vector>

#include "xng/resource/resourcebase.hpp"

#include "material.hpp"

#include "xng/rendergraph/shader/shaderattributelayout.hpp"

namespace xng {
    struct XENGINE_EXPORT Mesh final : ResourceBase {
        RESOURCE_TYPENAME(Mesh)

        /**
         * A quad mesh which covers the viewport in normalized screen coordinates
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec2 uv;
         *
         * @return
         */
        static Mesh normalizedQuad();

        /**
         * A subdivided normalized cube mesh.
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec3 normal;
         *  layout (location = 2) in vec2 uv;
         *
         * @param subdivisions Number of subdivisions per face edge (e.g., 16 creates 16x16 quads per face)
         * @return
         */
        static Mesh normalizedCube(int subdivisions = 1);

        /**
         * A sphere mesh.
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec3 normal;
         *  layout (location = 2) in vec2 uv;
         *
         * @param radius The radius of the sphere
         * @param latitudes The number of latitudes
         * @param longitudes The number of longitudes
         * @return
         */
        static Mesh sphere(float radius, int latitudes, int longitudes);

        enum Primitive : int {
            POINTS = 1,
            LINES = 2,
            TRIANGLES = 3,
            QUAD = 4
        };

        struct VertexWeight {
            size_t vertex{}; // The index of the vertex in the mesh
            float weight{}; // The weight
        };

        struct MorphTarget {
            std::vector<Vec3f> positions;
            std::vector<Vec3f> normals;
            std::vector<Vec3f> tangents;
            std::vector<Vec3f> bitangents;
            std::vector<Vec2f> uvs;
        };

        Primitive primitive = POINTS;

        std::vector<Vec3f> positions;
        std::vector<Vec3f> normals;
        std::vector<Vec2f> uvs;
        std::vector<Vec3f> tangents;
        std::vector<Vec3f> bitangents;

        std::vector<unsigned int> indices;

        std::unordered_map<std::string, std::vector<VertexWeight>> boneWeights; // The list of bone names influencing the mesh and their weights

        std::vector<MorphTarget> morphTargets; // The list of morph targets

        Mesh() = default;

        ~Mesh() override = default;

        std::unique_ptr<ResourceBase> clone() override {
            return std::make_unique<Mesh>(*this);
        }
    };
}

#endif //XENGINE_MESH_HPP
