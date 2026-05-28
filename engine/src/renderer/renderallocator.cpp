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

#include "xng/renderer/renderallocator.hpp"

namespace xng {
    RenderAllocator::RenderAllocator(rg::Heap &heap, const size_t streamingBudget)
        : heap(heap),
          chunkStreamer(heap, 256 * 1024, streamingBudget / (256 * 1024)),
          shaderMeshStream(heap, chunkStreamer),
          transformStream(heap, chunkStreamer),
          materialStream(heap, chunkStreamer),
          skeletonStream(heap, chunkStreamer),
          pointLightStream(heap, chunkStreamer),
          spotLightStream(heap, chunkStreamer),
          directionalLightStream(heap, chunkStreamer),
          meshStream(heap, chunkStreamer),
          textureStream(heap, chunkStreamer) {
    }

    RenderObjectHandle<RenderTexture> RenderAllocator::createTexture(const Vec2i &resolution) {
        return std::make_shared<RenderTexture>(allocateId(), textureStream, resolution);
    }

    RenderObjectHandle<RenderMaterial> RenderAllocator::createMaterial(const ColorRGBA &albedo,
                                                                       float metallic,
                                                                       float roughness,
                                                                       float ambientOcclusion,
                                                                       const Vec4f &normalIntensity,
                                                                       RenderObjectHandle<RenderTexture> albedoTexture,
                                                                       SamplingProperties albedoProperties,
                                                                       RenderObjectHandle<RenderTexture>
                                                                       metallicTexture,
                                                                       SamplingProperties metallicProperties,
                                                                       RenderObjectHandle<RenderTexture>
                                                                       roughnessTexture,
                                                                       SamplingProperties roughnessProperties,
                                                                       RenderObjectHandle<RenderTexture>
                                                                       ambientOcclusionTexture,
                                                                       SamplingProperties ambientOcclusionProperties,
                                                                       RenderObjectHandle<RenderTexture>
                                                                       normalTexture,
                                                                       SamplingProperties normalProperties) {
        return std::make_shared<RenderMaterial>(allocateId(),
                                                materialStream,
                                                albedo,
                                                metallic,
                                                roughness,
                                                ambientOcclusion,
                                                std::move(albedoTexture),
                                                albedoProperties,
                                                std::move(metallicTexture),
                                                metallicProperties,
                                                std::move(roughnessTexture),
                                                roughnessProperties,
                                                std::move(ambientOcclusionTexture),
                                                ambientOcclusionProperties,
                                                std::move(normalTexture),
                                                normalProperties,
                                                normalIntensity);
    }

    RenderObjectHandle<RenderSkeleton> RenderAllocator::createSkeleton(const std::vector<std::string> &boneNames) {
        return std::make_shared<RenderSkeleton>(allocateId(), skeletonStream, boneNames);
    }

    RenderObjectHandle<RenderMesh> RenderAllocator::createMesh(const Mesh &mesh,
                                                               RenderObjectHandle<RenderSkeleton> skeleton) {
        return std::make_shared<RenderMesh>(allocateId(), meshStream, mesh, std::move(skeleton));
    }

    RenderObjectHandle<RenderModel> RenderAllocator::createModel(std::vector<RenderObjectHandle<RenderMesh> > meshes,
                                                                 RenderObjectHandle<RenderMaterial> material,
                                                                 RenderPath renderPath,
                                                                 ShadingModel shadingModel,
                                                                 bool receiveShadows,
                                                                 bool castShadows) {
        return std::make_shared<RenderModel>(allocateId(),
                                             transformStream,
                                             shaderMeshStream,
                                             std::move(meshes),
                                             std::move(material),
                                             renderPath,
                                             shadingModel,
                                             receiveShadows,
                                             castShadows);
    }

    RenderObjectHandle<RenderPointLight> RenderAllocator::createPointLight() {
        return std::make_shared<RenderPointLight>(allocateId(), pointLightStream);
    }

    RenderObjectHandle<RenderSpotLight> RenderAllocator::createSpotLight() {
        return std::make_shared<RenderSpotLight>(allocateId(), spotLightStream);
    }

    RenderObjectHandle<RenderDirectionalLight> RenderAllocator::createDirectionalLight() {
        return std::make_shared<RenderDirectionalLight>(allocateId(), directionalLightStream);
    }

    RenderObjectHandle<RenderPaint> RenderAllocator::createPaint() {
        return nullptr;
    }

    RenderObjectHandle<RenderCanvas> RenderAllocator::createCanvas() {
        return nullptr;
    }

    void RenderAllocator::destroy(const RenderObject &object) {
        deallocateId(object.getId());
    }

    static void concatPasses(const std::vector<rg::TransferPass> &passes, std::vector<rg::TransferPass> &out) {
        out.insert(out.end(), passes.begin(), passes.end());
    }

    [[nodiscard]] RenderAllocator::Buffers RenderAllocator::commit(rg::GraphBuilder &graph) {
        std::vector<rg::TransferPass> streamPasses;
        concatPasses(shaderMeshStream.commit(graph), streamPasses);
        concatPasses(transformStream.commit(graph), streamPasses);
        concatPasses(materialStream.commit(graph), streamPasses);
        concatPasses(skeletonStream.commit(graph), streamPasses);
        concatPasses(pointLightStream.commit(graph), streamPasses);
        concatPasses(spotLightStream.commit(graph), streamPasses);
        concatPasses(directionalLightStream.commit(graph), streamPasses);
        concatPasses(meshStream.commit(graph), streamPasses);
        concatPasses(textureStream.commit(graph), streamPasses);
        chunkStreamer.commit((std::move(streamPasses)));
        return {
            shaderMeshStream.getBuffer(),
            transformStream.getBuffer(),
            materialStream.getBuffer(),
            skeletonStream.getBuffer(),
            pointLightStream.getBuffer(),
            spotLightStream.getBuffer(),
            directionalLightStream.getBuffer(),
            meshStream.getVertexBuffers(),
            meshStream.getIndexBuffer(),
            meshStream.getSkinnedBindPosBuffer(),
            meshStream.getSkinnedBoneIndicesBuffer(),
            meshStream.getSkinnedBoneWeightsBuffer(),
            textureStream.getTextures()
        };
    }
}
