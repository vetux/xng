/**
 *  This file is part of xEngine, a C++ game engine library.
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

#ifndef XENGINE_MESH_HPP
#define XENGINE_MESH_HPP

#include <utility>
#include <vector>
#include <typeindex>
#include <cstdint>

#include "xng/math/vector3.hpp"
#include "xng/math/vector2.hpp"

#include "xng/geometry/vertex.hpp"
#include "xng/geometry/primitive.hpp"

#include "xng/resource/resource.hpp"

#include "xng/animation/skeletal/rig.hpp"
#include "xng/animation/skeletal/riganimation.hpp"

#include "xng/gpu/vertexarrayobjectdesc.hpp"

namespace xng {
    struct XENGINE_EXPORT Mesh : public Resource {

        /**
         * Create a standard vertex buffer description which is the format of meshes returned by the resource abstraction.
         *
         * eg. GLSL:
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec3 normal;
         *  layout (location = 2) in vec2 uv;
         *  layout (location = 3) in vec3 tangent;
         *  layout (location = 4) in vec3 bitangent;
         *  layout (location = 5) in ivec4 boneIds;
         *  layout (location = 6) in vec4 boneWeights;
         *
         * @param mesh
         * @param bufferType
         *
         * @return
         */
        static VertexArrayObjectDesc getDefaultVertexArrayObjectDesc() {
            const std::vector<VertexAttribute> layout = {
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::SIGNED_INT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
            };

            return {
                .vertexLayout = layout
            };
        }

        /**
         * eg. GLSL:
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec3 normal;
         *  layout (location = 2) in vec2 uv;
         *  layout (location = 3) in vec3 tangent;
         *  layout (location = 4) in vec3 bitangent;
         *  layout (location = 5) in ivec4 boneIds;
         *  layout (location = 6) in vec4 boneWeights;
         *  -- Instance Start --
         *  layout (location = 7) in vec4 instanceRow0;
         *  layout (location = 8) in vec4 instanceRow1;
         *  layout (location = 9) in vec4 instanceRow2;
         *  layout (location = 10) in vec4 instanceRow3;
         *
         * @param mesh
         * @param offsets
         * @return
         */
        static VertexArrayObjectDesc createInstancedVertexArrayBufferDesc() {
            const std::vector<VertexAttribute> layout = {
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::SIGNED_INT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
            };

            const std::vector<VertexAttribute> instanceLayout = {
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT)
            };

            return {
                    .vertexLayout = layout,
                    .instanceArrayLayout = instanceLayout
            };
        }

        /**
         * A quad mesh which covers the viewport in normalized screen coordinates
         * @return
         */
        static const Mesh &normalizedQuad();

        static const Mesh &normalizedCube();

        ~Mesh() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Mesh>(*this);
        }

        std::type_index getTypeIndex() override;

        Primitive primitive = POINTS;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        Rig rig; // If rig is assigned the vertex bone ids are indices into rig.getBones()

        Mesh() = default;

        Mesh(Primitive primitive, std::vector<Vertex> vertices)
                : primitive(primitive), vertices(std::move(vertices)), indices() {}

        Mesh(Primitive primitive, std::vector<Vertex> vertices, std::vector<unsigned int> indices)
                : primitive(primitive), vertices(std::move(vertices)), indices(std::move(indices)) {}

        Mesh(Primitive primitive, std::vector<Vertex> vertices, std::vector<unsigned int> indices, Rig rig)
                : primitive(primitive),
                  vertices(std::move(vertices)),
                  indices(std::move(indices)),
                  rig(std::move(rig)) {}

        size_t polyCount() const {
            if (indices.empty())
                return vertices.size() / primitive;
            else
                return indices.size() / primitive;
        }
    };
}

#endif //XENGINE_MESH_HPP
