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

#include "xng/renderer/scenestreamer.hpp"

namespace xng {
    SceneStreamer::SceneStreamer(rg::Heap &heap, const size_t streamingBudget)
        : heap(heap),
          chunkStreamer(heap, 256 * 1024, streamingBudget / (256 * 1024)),
          transformStream(heap, chunkStreamer),
          materialStream(heap, chunkStreamer),
          skeletonStream(heap, chunkStreamer),
          pointLightStream(heap, chunkStreamer),
          spotLightStream(heap, chunkStreamer),
          directionalLightStream(heap, chunkStreamer),
          meshStream(heap, chunkStreamer),
          textureStream(heap, chunkStreamer) {
    }

    RenderObjectHandle<RenderTexture> SceneStreamer::createTexture(const Vec2i &resolution) {
        return std::make_shared<RenderTexture>(textureStream, resolution);
    }

    RenderObjectHandle<RenderMaterial> SceneStreamer::createMaterial(const ColorRGBA &albedo,
                                                                     float metallic,
                                                                     float roughness,
                                                                     float ambientOcclusion,
                                                                     const Vec4f &normalIntensity,
                                                                     RenderObjectHandle<RenderTexture> albedoTexture,
                                                                     RenderObjectHandle<RenderTexture> metallicTexture,
                                                                     RenderObjectHandle<RenderTexture> roughnessTexture,
                                                                     RenderObjectHandle<RenderTexture>
                                                                     ambientOcclusionTexture,
                                                                     RenderObjectHandle<RenderTexture> normalTexture) {
        return std::make_shared<RenderMaterial>(materialStream,
                                                albedo,
                                                metallic,
                                                roughness,
                                                ambientOcclusion,
                                                std::move(albedoTexture),
                                                std::move(metallicTexture),
                                                std::move(roughnessTexture),
                                                std::move(ambientOcclusionTexture),
                                                std::move(normalTexture),
                                                normalIntensity);
    }

    RenderObjectHandle<RenderSkeleton> SceneStreamer::createSkeleton(const std::vector<std::string> &boneNames) {
        return std::make_shared<RenderSkeleton>(skeletonStream, boneNames);
    }

    RenderObjectHandle<RenderMesh> SceneStreamer::createMesh(const Mesh &mesh,
                                                             RenderObjectHandle<RenderSkeleton> skeleton) {
        return std::make_shared<RenderMesh>(meshStream, mesh, std::move(skeleton));
    }

    RenderObjectHandle<RenderModel> SceneStreamer::createModel(std::vector<RenderObjectHandle<RenderMesh> > meshes,
                                                               RenderObjectHandle<RenderMaterial> material,
                                                               ShadingModel shadingModel,
                                                               bool receiveShadows,
                                                               bool castShadows) {
        return std::make_shared<RenderModel>(transformStream,
                                             std::move(meshes),
                                             std::move(material),
                                             shadingModel,
                                             receiveShadows,
                                             castShadows);
    }

    RenderObjectHandle<RenderPointLight> SceneStreamer::createPointLight() {
        return std::make_shared<RenderPointLight>(pointLightStream);
    }

    RenderObjectHandle<RenderSpotLight> SceneStreamer::createSpotLight() {
        return std::make_shared<RenderSpotLight>(spotLightStream);
    }

    RenderObjectHandle<RenderDirectionalLight> SceneStreamer::createDirectionalLight() {
        return std::make_shared<RenderDirectionalLight>(directionalLightStream);
    }

    static void concatPasses(const std::vector<rg::TransferPass> &passes, std::vector<rg::TransferPass> &out) {
        out.insert(out.end(), passes.begin(), passes.end());
    }

    [[nodiscard]] SceneStreamer::Buffers SceneStreamer::commit(rg::GraphBuilder &graph) {
        std::vector<rg::TransferPass> streamPasses;
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
