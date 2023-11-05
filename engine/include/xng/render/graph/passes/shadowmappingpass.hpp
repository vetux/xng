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

#ifndef XENGINE_SHADOWMAPPINGPASS_HPP
#define XENGINE_SHADOWMAPPINGPASS_HPP

#include "xng/render/graph/framegraphpass.hpp"
#include "xng/render/pointlight.hpp"
#include "xng/render/meshallocator.hpp"
#include "xng/render/scene.hpp"

namespace xng {
    /**
     * The shadow mapping pass creates the shadow mapping textures.
     *
     * Writes SLOT_SHADOW_MAP_POINT
     */
    class XENGINE_EXPORT ShadowMappingPass : public FrameGraphPass {
    public:
        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources,
                     const std::vector<std::reference_wrapper<CommandQueue>> &renderQueues,
                     const std::vector<std::reference_wrapper<CommandQueue>> &computeQueues,
                     const std::vector<std::reference_wrapper<CommandQueue>> &transferQueues) override;

        std::type_index getTypeIndex() const override;

        void setShadowResolution(const Vec2i &value){
            resolution = value;
        }

        const Vec2i &getShadowResolution() const {
            return resolution;
        }

        float getNearPlane() const;

        void setNearPlane(float nearPlane);

        float getFarPlane() const;

        void setFarPlane(float farPlane);

    private:
        Vec2i resolution = Vec2i(2048, 2048);
        float nearPlane = 0.1;
        float farPlane = 1000;

        std::vector<Scene::Node> pointLightNodes;

        size_t currentVertexBufferSize{};
        size_t currentIndexBufferSize{};

        std::vector<Scene::Node> meshNodes;

        MeshAllocator meshAllocator;

        FrameGraphResource targetRes;
        FrameGraphResource renderPassRes;
        FrameGraphResource shaderBufferRes;
        FrameGraphResource lightBufferRes;
        FrameGraphResource boneBufferRes;
        FrameGraphResource commandBufferRes;

        FrameGraphResource renderPipelineRes;

        FrameGraphResource vertexBufferRes;
        FrameGraphResource indexBufferRes;
        FrameGraphResource vertexArrayObjectRes;

        FrameGraphResource staleVertexBuffer;
        FrameGraphResource staleIndexBuffer;

        FrameGraphResource pointLightShadowMapRes;

        FrameGraphResource textureRes;

        std::set<Uri> usedMeshes;

        bool bindVao = true;
    };
}
#endif //XENGINE_SHADOWMAPPINGPASS_HPP
