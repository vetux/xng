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

#ifndef XENGINE_ASSETSCENE_HPP
#define XENGINE_ASSETSCENE_HPP

#include <optional>
#include <vector>

#include "xng/assets/nodeanimation.hpp"
#include "xng/assets/material.hpp"
#include "xng/assets/mesh.hpp"

#include "xng/math/transform.hpp"

#include "xng/resource/resourcebase.hpp"
#include "xng/resource/resourcehandle.hpp"

namespace xng {
    struct AssetScene final : ResourceBase {
        RESOURCE_TYPENAME(AssetScene)

        struct MorphAnimation {
            struct Keyframe {
                double time;
                std::vector<std::pair<size_t, float> > weights; // target index + weight
            };

            std::vector<Keyframe> keyframes;
        };

        struct Node {
            struct MeshData {
                ResourceHandle<Mesh> mesh;
                ResourceHandle<Material> material;
                std::optional<MorphAnimation> morphAnimation;
            };

            std::string name;

            Transform transform; // local transform relative to parent

            std::vector<MeshData> meshes;
            std::optional<Mat4f> inverseBind; // The bone inverse bind matrix

            std::vector<Node> children;
        };

        Node root;
        std::vector<ResourceHandle<NodeAnimation> > nodeAnimations;

        std::unique_ptr<ResourceBase> clone() override {
            return std::make_unique<AssetScene>(*this);
        }
    };
}

#endif //XENGINE_ASSETSCENE_HPP
