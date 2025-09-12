/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_SKINNEDMESH_HPP
#define XENGINE_SKINNEDMESH_HPP

#include <utility>

#include "xng/render/scene/mesh.hpp"

namespace xng {
    class SkinnedMesh : public Mesh {
    public:
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
        static FGAttributeLayout getDefaultVertexLayout() {
            return FGAttributeLayout({
                FGShaderValue::vec3(),
                FGShaderValue::vec3(),
                FGShaderValue::vec2(),
                FGShaderValue::vec3(),
                FGShaderValue::vec3(),
                FGShaderValue::vec4(),
                FGShaderValue::vec4()
            });
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
        static FGAttributeLayout getDefaultInstanceLayout() {
            return FGAttributeLayout({
                FGShaderValue::vec3(),
                FGShaderValue::vec3(),
                FGShaderValue::vec2(),
                FGShaderValue::vec3(),
                FGShaderValue::vec3(),
                FGShaderValue::vec4(),
                FGShaderValue::vec4(),

                FGShaderValue::vec4(),
                FGShaderValue::vec4(),
                FGShaderValue::vec4(),
                FGShaderValue::vec4()
            });
        }

        SkinnedMesh() = default;

        explicit SkinnedMesh(const Mesh &mesh)
            : Mesh(mesh), rig() {
        }

        SkinnedMesh(Primitive primitive, std::vector<Vertex> vertices)
            : Mesh(primitive, std::move(vertices), {}), rig() {
        }

        SkinnedMesh(Primitive primitive, std::vector<Vertex> vertices, std::vector<unsigned int> indices)
            : Mesh(primitive, std::move(vertices), std::move(indices)), rig() {
        }

        SkinnedMesh(Primitive primitive, std::vector<Vertex> vertices, std::vector<unsigned int> indices, Rig rig)
            : Mesh(primitive, std::move(vertices), std::move(indices)), rig(std::move(rig)) {
        }

        SkinnedMesh(const SkinnedMesh &other) = default;

        SkinnedMesh(SkinnedMesh &&other) = default;

        ~SkinnedMesh() override = default;

        SkinnedMesh &operator=(const SkinnedMesh &other) = default;

        SkinnedMesh &operator=(SkinnedMesh &&other) = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<SkinnedMesh>(*this);
        }

        std::type_index getTypeIndex() const override {
            return typeid(SkinnedMesh);
        }

        Rig rig;
    };
}
#endif //XENGINE_SKINNEDMESH_HPP
