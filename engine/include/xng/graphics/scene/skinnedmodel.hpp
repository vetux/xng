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

#ifndef XENGINE_SKINNEDMESH_HPP
#define XENGINE_SKINNEDMESH_HPP

#include <utility>
#include <unordered_set>

#include "xng/graphics/scene/mesh.hpp"

#include "xng/animation/skeletal/rig.hpp"

namespace xng {
    /**
     * A skinned model with the corresponding rig.
     *
     * Animations can be reused across skinned models as long as all bone names referenced by the animation are present in the rig.
     */
    class SkinnedModel final : public Resource {
    public:
        RESOURCE_TYPENAME(SkinnedModel)

        struct SubMesh {
            Mesh mesh;
            ResourceHandle<Material> material;
            std::unordered_set<std::string> bones;
        };

        /**
         * The standard layout for skinned model meshes.
         *
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec3 normal;
         *  layout (location = 2) in vec2 uv;
         *  layout (location = 3) in vec3 tangent;
         *  layout (location = 4) in vec3 bitangent;
         *  layout (location = 5) in ivec4 boneIds;
         *  layout (location = 6) in vec4 boneWeights;
         *
         * @return
         */
        static ShaderAttributeLayout getVertexLayout() {
            return ShaderAttributeLayout({
                {"position", ShaderPrimitiveType::vec3()},
                {"normal", ShaderPrimitiveType::vec3()},
                {"uv", ShaderPrimitiveType::vec2()},
                {"tangent", ShaderPrimitiveType::vec3()},
                {"bitangent", ShaderPrimitiveType::vec3()},
                {"boneIds", ShaderPrimitiveType::ivec4()},
                {"boneWeights", ShaderPrimitiveType::vec4()}
            });
        }

        Rig rig;
        std::vector<SubMesh> subMeshes;

        SkinnedModel() = default;

        SkinnedModel(Rig rig, std::vector<SubMesh> subMeshes)
            : rig(std::move(rig)), subMeshes(std::move(subMeshes)) {
        }

        ~SkinnedModel() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<SkinnedModel>(*this);
        }

        bool isLoaded() const override {
            for (auto &subMesh: subMeshes) {
                if (!subMesh.material.isLoaded())
                    return false;
            }
            return true;
        }
    };
}
#endif //XENGINE_SKINNEDMESH_HPP
