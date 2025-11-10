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

#include "xng/resource/resource.hpp"

#include "xng/graphics/scene/material.hpp"

#include "xng/rendergraph/shader/shaderattributelayout.hpp"
#include "xng/rendergraph/renderprimitive.hpp"

namespace xng {
    struct XENGINE_EXPORT Mesh : Resource {
        RESOURCE_TYPENAME(Mesh)

        /**
         * A quad mesh which covers the viewport in normalized screen coordinates
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec2 uv;
         *
         * @return
         */
        static const Mesh &normalizedQuad();

        /**
         * A cube mesh with an extent of 1.
        *   layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec3 normal;
         *  layout (location = 2) in vec2 uv;
         *  layout (location = 3) in vec3 tangent;
         *  layout (location = 4) in vec3 bitangent;
         *
         * @return
         */
        static const Mesh &normalizedCube();

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

        RenderPrimitive primitive = POINTS;
        std::vector<uint8_t> vertices;
        std::vector<unsigned int> indices;
        ShaderAttributeLayout vertexLayout;

        Mesh() = default;

        Mesh(const RenderPrimitive primitive, std::vector<uint8_t> vertices)
            : primitive(primitive), vertices(std::move(vertices)) {
        }

        Mesh(const RenderPrimitive primitive, std::vector<uint8_t> vertices, std::vector<unsigned int> indices)
            : primitive(primitive), vertices(std::move(vertices)), indices(std::move(indices)) {
        }

        ~Mesh() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Mesh>(*this);
        }

        size_t polyCount() const {
            if (indices.empty())
                return vertices.size() / vertexLayout.getLayoutSize() / primitive;
            return indices.size() / primitive;
        }
    };
}

#endif //XENGINE_MESH_HPP
