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

#ifndef XENGINE_MESH_HPP
#define XENGINE_MESH_HPP

#include <utility>
#include <vector>
#include <typeindex>
#include <cstdint>

#include "xng/math/vector3.hpp"
#include "xng/math/vector2.hpp"

#include "xng/render/geometry/vertex.hpp"
#include "xng/render/geometry/primitive.hpp"

#include "xng/resource/resource.hpp"

#include "xng/animation/skeletal/rig.hpp"
#include "xng/animation/skeletal/riganimation.hpp"

#include "xng/gpu/vertexarrayobjectdesc.hpp"

#include "material.hpp"

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
         *
         * @param mesh
         * @param bufferType
         *
         * @return
         */
        static VertexLayout getDefaultVertexLayout() {
            const std::vector<VertexAttribute> layout = {
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
            };

            return VertexLayout(layout);
        }

        /**
         * eg. GLSL:
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec3 normal;
         *  layout (location = 2) in vec2 uv;
         *  layout (location = 3) in vec3 tangent;
         *  layout (location = 4) in vec3 bitangent;
         *  -- Instance Start --
         *  layout (location = 5) in vec4 instanceRow0;
         *  layout (location = 6) in vec4 instanceRow1;
         *  layout (location = 7) in vec4 instanceRow2;
         *  layout (location = 8) in vec4 instanceRow3;
         *
         * @param mesh
         * @param offsets
         * @return
         */
        static VertexLayout getDefaultInstanceLayout() {
            const std::vector<VertexAttribute> instanceLayout = {
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),

                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT)
            };

            return VertexLayout(instanceLayout);
        }

        /**
         * A quad mesh which covers the viewport in normalized screen coordinates
         * @return
         */
        static const Mesh &normalizedQuad();

        static const Mesh &normalizedCube();

        static Mesh sphere(float radius, int latitudes, int longitudes);

        Primitive primitive = POINTS;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        VertexLayout vertexLayout;

        ResourceHandle<Material> material;

        std::vector<std::string> bones;

        std::vector<Mesh> subMeshes;

        Mesh() = default;

        Mesh(Primitive primitive, std::vector<Vertex> vertices)
                : primitive(primitive), vertices(std::move(vertices)), indices() {}

        Mesh(Primitive primitive, std::vector<Vertex> vertices, std::vector<unsigned int> indices)
                : primitive(primitive), vertices(std::move(vertices)), indices(std::move(indices)) {}

        Mesh(Primitive primitive,
             std::vector<Vertex> vertices,
             std::vector<unsigned int> indices,
             ResourceHandle<Material> material)
                : primitive(primitive),
                  vertices(std::move(vertices)),
                  indices(std::move(indices)),
                  material(std::move(material)) {}

        Mesh(Primitive primitive,
             std::vector<Vertex> vertices,
             std::vector<unsigned int> indices,
             ResourceHandle<Material> material,
             std::vector<Mesh> subMeshes)
                : primitive(primitive),
                  vertices(std::move(vertices)),
                  indices(std::move(indices)),
                  material(std::move(material)),
                  subMeshes(std::move(subMeshes)) {}

        Mesh(const Mesh &other) = default;

        Mesh(Mesh &&other) = default;

        ~Mesh() override = default;

        Mesh &operator=(const Mesh &other) = default;

        Mesh &operator=(Mesh &&other) = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Mesh>(*this);
        }

        std::type_index getTypeIndex() const override;

        size_t polyCount() const {
            if (indices.empty())
                return vertices.size() / primitive;
            else
                return indices.size() / primitive;
        }

        bool isLoaded() const override {
            if (!material.isLoaded() || !material.get().isLoaded()) {
                return false;
            } else {
                for (auto &mesh: subMeshes) {
                    if (!mesh.isLoaded()) {
                        return false;
                    }
                }
                return true;
            }
        }
    };
}

#endif //XENGINE_MESH_HPP
