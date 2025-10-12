/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_STATICMODEL_HPP
#define XENGINE_STATICMODEL_HPP

#include "xng/graphics/scene/mesh.hpp"

namespace xng {
    /**
     * A non skinned static model.
     */
    class StaticModel final : public Resource {
    public:
        RESOURCE_TYPENAME(StaticModel)

        struct SubMesh {
            Mesh mesh;
            ResourceHandle<Material> material;
        };

        /**
         * The standard vertex layout for static model meshes.
         *
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec3 normal;
         *  layout (location = 2) in vec2 uv;
         *  layout (location = 3) in vec3 tangent;
         *  layout (location = 4) in vec3 bitangent;
         *
         * @param mesh
         * @param bufferType
         * @return
         */
        static ShaderAttributeLayout getVertexLayout() {
            return ShaderAttributeLayout({
                {"position", ShaderDataType::vec3()},
                {"normal", ShaderDataType::vec3()},
                {"uv", ShaderDataType::vec2()},
                {"tangent", ShaderDataType::vec3()},
                {"bitangent", ShaderDataType::vec3()}
            });
        }

        std::vector<SubMesh> subMeshes;

        StaticModel() = default;

        explicit StaticModel(std::vector<SubMesh> subMeshes)
            : subMeshes(std::move(subMeshes)) {
        }

        ~StaticModel() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<StaticModel>(*this);
        }

        bool isLoaded() const override {
            for (auto &subMesh : subMeshes) {
                if (!subMesh.material.isLoaded())
                    return false;
            }
            return true;
        }
    };
}

#endif //XENGINE_STATICMODEL_HPP
