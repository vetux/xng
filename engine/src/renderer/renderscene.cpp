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

#include "xng/renderer/renderscene.hpp"

#include "xng/adapters/opengl/opengl.hpp"
#include "xng/renderer/pipeline/indirect/renderpipelineindirect.hpp"

namespace xng {
    RenderScene::RenderScene(rg::Runtime &runtime,
                             ChunkStreamer &chunkStreamer,
                             const size_t tileSize,
                             const size_t tileBorder,
                             const float maxAnisotropy)
        : runtime(runtime),
          chunkStreamer(chunkStreamer),
          skeletonStreamer(runtime.getResourceHeap(), chunkStreamer),
          meshStreamer(runtime.getResourceHeap(), chunkStreamer),
          virtualTextureStreamer(runtime, chunkStreamer, tileSize, tileBorder, maxAnisotropy),
          pointLightBuffer(runtime.getResourceHeap(), chunkStreamer, rg::Buffer::CAPABILITY_STORAGE),
          directionalLightBuffer(runtime.getResourceHeap(), chunkStreamer, rg::Buffer::CAPABILITY_STORAGE),
          spotLightBuffer(runtime.getResourceHeap(), chunkStreamer, rg::Buffer::CAPABILITY_STORAGE),
          unitQuadMesh(createMesh(Mesh::normalizedQuad())),
          unitCubeMesh(createMesh(Mesh::normalizedCube())) {
    }

    void RenderScene::setCamera(const Camera &value) {
        camera = value;

        pbrDeferredPipeline->setCamera(camera.getTransform().getPosition(), camera.getView(), camera.getProjection());
        pbrForwardPipeline->setCamera(camera.getTransform().getPosition(), camera.getView(), camera.getProjection());

        for (auto &pair: shaders) {
            pair.second.getPipeline()->setCamera(camera.getTransform().getPosition(),
                                                 camera.getView(),
                                                 camera.getProjection());
        }
    }

    const Camera &RenderScene::getCamera() const {
        return camera;
    }

    RenderObjectHandle<RenderTexture> RenderScene::createTexture(const ImageRGBA &image,
                                                                 const WrappingMethod wrapping,
                                                                 const unsigned int mipLevels) {
        const auto id = allocateID();
        const auto size = image.getResolution();
        const auto maxMip = mipLevels - 1;
        const auto textureHandle = virtualTextureStreamer.create(std::make_shared<ImageTileLoader>(image,
            maxMip + 1,
            virtualTextureStreamer.getTileSize(),
            virtualTextureStreamer.getTileBorder(),
            wrapping,
            runtime));
        textures.emplace(id,
                         RenderTexture(virtualTextureStreamer,
                                       textureHandle,
                                       size,
                                       maxMip));
        types[id] = RenderObject::RENDER_TEXTURE;
        return {this, id, textures.at(id)};
    }

    RenderObjectHandle<RenderTexture> RenderScene::createTexture(const ImageRGBA &image,
                                                                 const WrappingMethod wrapping) {
        const auto id = allocateID();
        const auto size = image.getResolution();
        const auto maxMip = rg::Texture::calculateMipLevels(image.getResolution()) - 1;
        const auto textureHandle = virtualTextureStreamer.create(std::make_shared<ImageTileLoader>(image,
            maxMip + 1,
            virtualTextureStreamer.getTileSize(),
            virtualTextureStreamer.getTileBorder(),
            wrapping,
            runtime));
        textures.emplace(id,
                         RenderTexture(virtualTextureStreamer,
                                       textureHandle,
                                       size,
                                       maxMip));
        types[id] = RenderObject::RENDER_TEXTURE;
        return {this, id, textures.at(id)};
    }

    RenderObjectHandle<RenderTexture> RenderScene::createTexture(const std::shared_ptr<TileLoader> &tileLoader) {
        const auto id = allocateID();
        const auto size = tileLoader->getSize();
        const auto maxMip = tileLoader->getMipLevels() - 1;
        const auto textureHandle = virtualTextureStreamer.create(tileLoader);
        textures.emplace(id,
                         RenderTexture(virtualTextureStreamer,
                                       textureHandle,
                                       size,
                                       maxMip));
        types[id] = RenderObject::RENDER_TEXTURE;
        return {this, id, textures.at(id)};
    }

    RenderObjectHandle<RenderSkeleton> RenderScene::createSkeleton(const std::vector<std::string> &boneNames) {
        const auto id = allocateID();
        auto skeletonHandle = skeletonStreamer.create(boneNames.size());
        std::unordered_map<std::string, unsigned int> boneOffsets;
        for (size_t i = 0; i < boneNames.size(); i++) {
            boneOffsets[boneNames.at(i)] = static_cast<unsigned int>(i);
        }
        skeletons.emplace(id, RenderSkeleton(skeletonStreamer, skeletonHandle, boneOffsets));
        types[id] = RenderObject::RENDER_SKELETON;
        return {this, id, skeletons.at(id)};
    }


    RenderObjectHandle<RenderMesh> RenderScene::createMesh(const Mesh &mesh) {
        const auto id = allocateID();
        const auto meshHandle = meshStreamer.create(mesh, {});
        meshes.emplace(id, RenderMesh(meshStreamer, meshHandle, {}));
        types[id] = RenderObject::RENDER_MESH;
        return {this, id, meshes.at(id)};
    }

    RenderObjectHandle<RenderMesh> RenderScene::createMesh(const Mesh &mesh,
                                                           RenderObjectHandle<RenderSkeleton> skeleton) {
        const auto id = allocateID();
        const auto meshHandle = meshStreamer.create(mesh, skeleton.get().getOffsets());
        meshes.emplace(id, RenderMesh(meshStreamer, meshHandle, skeleton));
        skinnedMeshes.insert(id);
        types[id] = RenderObject::RENDER_MESH;
        return {this, id, meshes.at(id)};
    }

    RenderObjectHandle<RenderShader> RenderScene::createShader(const std::vector<rg::Shader> &uShaders,
                                                               RenderPipeline::MaterialLayout materialLayout,
                                                               const rg::RasterPipeline::Configuration &
                                                               pipelineConfiguration) {
        const auto id = allocateID();
        auto pipeline = createPipeline(std::move(materialLayout));
        auto shader = pipeline->getCompiler().compile(uShaders,
                                                      pipelineConfiguration,
                                                      {
                                                          RenderPipelineShader::Attachment(
                                                              RenderPipelineShader::Attachment::ATTACHMENT_NATIVE,
                                                              rg::ShaderPrimitiveType::vec4(),
                                                              rg::RGBA8)
                                                      },
                                                      rg::DEPTH_32F,
                                                      {});
        shaders.emplace(id, RenderShader(std::move(pipeline), std::move(shader)));
        types[id] = RenderObject::RENDER_SHADER;
        return {this, id, shaders.at(id)};
    }

    RenderObjectHandle<RenderMaterial> RenderScene::createMaterial(const RenderObjectHandle<RenderShader> &shader) {
        const auto id = allocateID();
        auto materialHandle = shader.get().getPipeline()->createMaterial();
        materials.emplace(id, RenderMaterial(std::move(materialHandle), shader));
        types[id] = RenderObject::RENDER_MATERIAL;
        return {this, id, materials.at(id)};
    }

    RenderObjectHandle<RenderMaterial> RenderScene::createMaterial(const PBRMaterial &material,
                                                                   const RenderPath renderPath) {
        const auto id = allocateID();

        std::shared_ptr<RenderPipelineMaterial> materialHandle;
        switch (renderPath) {
            case RENDER_PATH_DEFERRED:
                materialHandle = pbrDeferredPipeline->createMaterial();
                break;
            case RENDER_PATH_FORWARD:
                materialHandle = pbrForwardPipeline->createMaterial();
                break;
        }

        materialHandle->update(material.getProperties(), material.getTextures());
        materials.emplace(id, RenderMaterial(std::move(materialHandle), SHADING_MODEL_PBR, renderPath));
        types[id] = RenderObject::RENDER_MATERIAL;
        return {this, id, materials.at(id)};
    }

    RenderObjectHandle<RenderModel> RenderScene::createModel(const RenderObjectHandle<RenderMaterial> &material,
                                                             const std::vector<RenderObjectHandle<RenderMesh> > &
                                                             mMeshes,
                                                             const bool castShadows,
                                                             const int sortPriority) {
        const auto id = allocateID();

        RenderPipeline *pipeline = nullptr;
        if (material.get().getShader().isAssigned()) {
            pipeline = material.get().getShader().get().getPipeline().get();
        } else {
            assert(material.get().getShadingModel() == SHADING_MODEL_PBR);
            switch (material.get().getRenderPath()) {
                case RENDER_PATH_FORWARD:
                    pipeline = pbrForwardPipeline.get();
                    break;
                case RENDER_PATH_DEFERRED:
                    pipeline = pbrDeferredPipeline.get();
                    break;
                default:
                    throw std::runtime_error("Unknown render path.");
            }
        }

        assert(pipeline != nullptr);

        const auto transformHandle = pipeline->createTransform();

        auto drawID = pipeline->addDrawCall(transformHandle,
                                            material.get().getHandle(),
                                            mMeshes,
                                            sortPriority);

        if (castShadows) {
            auto shadowTransform = shadowCastersPipeline->createTransform();
            const auto shadowDrawID = shadowCastersPipeline->addDrawCall(shadowTransform,
                                                                         mMeshes,
                                                                         0);
            models.emplace(id, RenderModel(transformHandle,
                                           material,
                                           drawID,
                                           mMeshes,
                                           std::move(shadowTransform),
                                           shadowDrawID));
        } else {
            models.emplace(id, RenderModel(transformHandle,
                                           material,
                                           drawID,
                                           mMeshes));
        }

        types[id] = RenderObject::RENDER_MODEL;
        return {this, id, models.at(id)};
    }

    RenderObjectHandle<RenderCanvas> RenderScene::createCanvas() {
        const auto id = allocateID();
        canvases.emplace(id, RenderCanvas(createPipeline(CanvasMaterial::getLayout())));
        types[id] = RenderObject::RENDER_CANVAS;
        return {this, id, canvases.at(id)};
    }

    RenderObjectHandle<RenderCanvas> RenderScene::createCanvas(const RenderObjectHandle<RenderTexture> &texture) {
        const auto id = allocateID();
        canvases.emplace(id, RenderCanvas(createPipeline(CanvasMaterial::getLayout()), texture));
        types[id] = RenderObject::RENDER_CANVAS;
        return {this, id, canvases.at(id)};
    }

    RenderObjectHandle<RenderPaint> RenderScene::createPaint(const RenderObjectHandle<RenderCanvas> &canvas,
                                                             const Vec2f &start,
                                                             const Vec2f &end,
                                                             const ColorRGBA &color,
                                                             const Vec2f &center,
                                                             const float rotation,
                                                             const int sortPriority) {
        const auto id = allocateID();
        const auto modelMatrix = MatrixMath::rotate(Vec3f(0, 0, rotation))
                                 * MatrixMath::translate(Vec3f(center.x, center.y, 0));

        CanvasMaterial canvasMaterial;
        canvasMaterial.setColor(color);

        Mesh m;
        m.primitive = Mesh::TRIANGLES;
        m.positions.emplace_back(start.x, start.y, 1);
        m.positions.emplace_back(end.x, end.y, 1);
        m.positions.emplace_back(end.x, end.y, 1);
        auto mesh = createMesh(m);

        auto transform = canvas.get().getPipeline()->createTransform();
        transform->setTransform(modelMatrix);

        auto material = canvas.get().getPipeline()->createMaterial();
        material->update(canvasMaterial.getProperties(), canvasMaterial.getTextures());

        const auto drawID = canvas.get().getPipeline()->addDrawCall(transform, material, {mesh}, sortPriority);

        paints.emplace(id, RenderPaint(canvas,
                                       std::move(transform),
                                       std::move(material),
                                       drawID,
                                       mesh));
        types[id] = RenderObject::RENDER_PAINT;
        return {this, id, paints.at(id)};
    }

    RenderObjectHandle<RenderPaint> RenderScene::createPaint(const RenderObjectHandle<RenderCanvas> &canvas,
                                                             const Vec2f &position,
                                                             const float size,
                                                             const ColorRGBA &color,
                                                             const int sortPriority) {
        const auto id = allocateID();
        const auto modelMatrix = MatrixMath::scale(Vec3f(size, size, 1))
                                 * MatrixMath::translate(Vec3f(position.x, position.y, 0));

        CanvasMaterial canvasMaterial;
        canvasMaterial.setColor(color);

        const auto mesh = unitQuadMesh;

        auto transform = canvas.get().getPipeline()->createTransform();
        transform->setTransform(modelMatrix);

        auto material = canvas.get().getPipeline()->createMaterial();
        material->update(canvasMaterial.getProperties(), canvasMaterial.getTextures());

        const auto drawID = canvas.get().getPipeline()->addDrawCall(transform, material, {mesh}, sortPriority);

        paints.emplace(id, RenderPaint(canvas,
                                       std::move(transform),
                                       std::move(material),
                                       drawID,
                                       mesh));
        types[id] = RenderObject::RENDER_PAINT;
        return {this, id, paints.at(id)};
    }

    RenderObjectHandle<RenderPaint> RenderScene::createPaint(const RenderObjectHandle<RenderCanvas> &canvas,
                                                             const Rectf &dstRect,
                                                             const ColorRGBA &color,
                                                             const Vec2f &center,
                                                             const float rotation,
                                                             const int sortPriority) {
        const auto id = allocateID();
        const auto modelMatrix = MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                                 * MatrixMath::rotate(Vec3f(0, 0, rotation))
                                 * MatrixMath::translate(Vec3f(center.x, center.y, 0))
                                 * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1));

        CanvasMaterial canvasMaterial;
        canvasMaterial.setColor(color);

        const auto mesh = unitQuadMesh;

        auto transform = canvas.get().getPipeline()->createTransform();
        transform->setTransform(modelMatrix);

        auto material = canvas.get().getPipeline()->createMaterial();
        material->update(canvasMaterial.getProperties(), canvasMaterial.getTextures());

        const auto drawID = canvas.get().getPipeline()->addDrawCall(transform, material, {mesh}, sortPriority);

        paints.emplace(id, RenderPaint(canvas,
                                       std::move(transform),
                                       std::move(material),
                                       drawID,
                                       mesh));
        types[id] = RenderObject::RENDER_PAINT;
        return {this, id, paints.at(id)};
    }

    RenderObjectHandle<RenderPaint> RenderScene::createPaint(const RenderObjectHandle<RenderCanvas> &canvas,
                                                             const Rectf &dstRect,
                                                             const RenderObjectHandle<RenderTexture> &texture,
                                                             const SamplingProperties &samplingProperties,
                                                             const ColorRGBA &mixColor,
                                                             const Vec4f &mix,
                                                             const Vec2f &center,
                                                             float rotation,
                                                             const int sortPriority) {
        const auto id = allocateID();
        const auto modelMatrix = MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                                 * MatrixMath::rotate(Vec3f(0, 0, rotation))
                                 * MatrixMath::translate(Vec3f(center.x, center.y, 0))
                                 * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1));

        CanvasMaterial canvasMaterial;
        canvasMaterial.setColor(mixColor);
        canvasMaterial.setMix(mix);
        canvasMaterial.setTexture(texture, samplingProperties);

        const auto mesh = unitQuadMesh;

        auto transform = canvas.get().getPipeline()->createTransform();
        transform->setTransform(modelMatrix);

        auto material = canvas.get().getPipeline()->createMaterial();
        material->update(canvasMaterial.getProperties(), canvasMaterial.getTextures());

        const auto drawID = canvas.get().getPipeline()->addDrawCall(transform, material, {mesh}, sortPriority);

        paints.emplace(id, RenderPaint(canvas,
                                       std::move(transform),
                                       std::move(material),
                                       drawID,
                                       mesh));
        types[id] = RenderObject::RENDER_PAINT;
        return {this, id, paints.at(id)};
    }

    RenderObjectHandle<RenderPointLight> RenderScene::createPointLight() {
        const auto id = allocateID();
        pointLights.emplace(id, RenderPointLight([this]() {
            reuploadPointLights = true;
        }));

        types[id] = RenderObject::RENDER_LIGHT_POINT;
        return {this, id, pointLights.at(id)};
    }

    RenderObjectHandle<RenderDirectionalLight> RenderScene::createDirectionalLight() {
        const auto id = allocateID();
        directionalLights.emplace(id, RenderDirectionalLight([this]() {
            reuploadDirectionalLights = true;
        }));
        types[id] = RenderObject::RENDER_LIGHT_DIRECTIONAL;
        return {this, id, directionalLights.at(id)};
    }

    RenderObjectHandle<RenderSpotLight> RenderScene::createSpotLight() {
        const auto id = allocateID();
        spotLights.emplace(id, RenderSpotLight([this]() {
            reuploadSpotLights = true;
        }));
        types[id] = RenderObject::RENDER_LIGHT_SPOT;
        return {this, id, spotLights.at(id)};
    }

    void RenderScene::commit(rg::GraphBuilder &graph, StreamerQueue &streamerQueue) {
        //TODO: Design better light buffer technique.

        // Light iteration in shaders is expensive.
        // One solution is storing lights in a separate buffer with gaps and using an index table.
        // This indirection is bad for cache performance, so lights should be in a single contiguous (uniform) buffer
        // that handles gaps by moving the lights / reallocating on grow / shrink.
        if (reuploadPointLights) {
            if (pointLightResident) {
                pointLightBuffer.release(pointLightBufferHandle);
            }

            std::vector<uint8_t> lightData;
            lightData.reserve(pointLights.size() * sizeof(ShaderPointLight::CPU));
            for (const auto &pair: pointLights) {
                lightData.insert(lightData.end(),
                                 reinterpret_cast<const uint8_t *>(&pair.second.getData()),
                                 reinterpret_cast<const uint8_t *>(&pair.second.getData()) + sizeof(
                                     ShaderPointLight::CPU));
            }
            pointLightBufferHandle = pointLightBuffer.upload(lightData, 0);
            pointLightBuffer.flush(pointLightBufferHandle);
            reuploadPointLights = false;
            pointLightResident = true;
        }

        if (reuploadDirectionalLights) {
            if (directionalLightResident) {
                directionalLightBuffer.release(directionalLightBufferHandle);
            }
            std::vector<uint8_t> lightData;
            lightData.reserve(directionalLights.size() * sizeof(ShaderDirectionalLight::CPU));
            for (const auto &pair: directionalLights) {
                lightData.insert(lightData.end(),
                                 reinterpret_cast<const uint8_t *>(&pair.second.getData()),
                                 reinterpret_cast<const uint8_t *>(&pair.second.getData()) + sizeof(
                                     ShaderDirectionalLight::CPU));
            }
            directionalLightBufferHandle = directionalLightBuffer.upload(lightData, 0);
            directionalLightBuffer.flush(directionalLightBufferHandle);
            reuploadDirectionalLights = false;
            directionalLightResident = true;
        }

        if (reuploadSpotLights) {
            if (spotLightResident) {
                spotLightBuffer.release(spotLightBufferHandle);
            }
            std::vector<uint8_t> lightData;
            lightData.reserve(spotLights.size() * sizeof(ShaderSpotLight::CPU));
            for (const auto &pair: spotLights) {
                lightData.insert(lightData.end(),
                                 reinterpret_cast<const uint8_t *>(&pair.second.getData()),
                                 reinterpret_cast<const uint8_t *>(&pair.second.getData()) + sizeof(
                                     ShaderSpotLight::CPU));
            }
            spotLightBufferHandle = spotLightBuffer.upload(lightData, 0);
            spotLightBuffer.flush(spotLightBufferHandle);
            reuploadSpotLights = false;
            spotLightResident = true;
        }

        pointLightBuffer.commit(graph);
        directionalLightBuffer.commit(graph);
        spotLightBuffer.commit(graph);

        skeletonStreamer.commit(graph);
        meshStreamer.commit(graph);
        virtualTextureStreamer.commit(graph);

        chunkStreamer.commit(graph, streamerQueue);
    }

    void RenderScene::prepare(rg::GraphBuilder &graph) {
        pbrDeferredPipeline->prepare(graph);
        pbrForwardPipeline->prepare(graph);
        shadowCastersPipeline->prepare(graph);

        for (auto &pair: shaders) {
            pair.second.getPipeline()->prepare(graph);
        }

        for (auto &pair: canvases) {
            pair.second.getPipeline()->prepare(graph);
        }
    }

    std::shared_ptr<RenderPipeline> RenderScene::createPipeline(RenderPipeline::MaterialLayout materialLayout) {
        // Here the scene will switch constructors based on platform support.
        return std::make_shared<RenderPipelineIndirect>(runtime,
                                                        chunkStreamer,
                                                        meshStreamer,
                                                        virtualTextureStreamer,
                                                        std::move(materialLayout),
                                                        RenderPipelineIndirect::getPrePassShader());
    }

    void RenderScene::incrementReference(const RenderObject::ID id) {
        refCounts.at(id)++;
    }

    void RenderScene::decrementReference(const RenderObject::ID id) {
        refCounts.at(id)--;
        if (refCounts.at(id) <= 0) {
            switch (types.at(id)) {
                case RenderObject::RENDER_TEXTURE:
                    destroyTexture(id);
                    break;
                case RenderObject::RENDER_SKELETON:
                    destroySkeleton(id);
                    break;
                case RenderObject::RENDER_MESH:
                    destroyMesh(id);
                    break;
                case RenderObject::RENDER_SHADER:
                    destroyShader(id);
                    break;
                case RenderObject::RENDER_MATERIAL:
                    destroyMaterial(id);
                    break;
                case RenderObject::RENDER_MODEL:
                    destroyModel(id);
                    break;
                case RenderObject::RENDER_CANVAS:
                    destroyCanvas(id);
                    break;
                case RenderObject::RENDER_PAINT:
                    destroyPaint(id);
                    break;
                case RenderObject::RENDER_LIGHT_POINT:
                    destroyPointLight(id);
                    break;
                case RenderObject::RENDER_LIGHT_DIRECTIONAL:
                    destroyDirectionalLight(id);
                    break;
                case RenderObject::RENDER_LIGHT_SPOT:
                    destroySpotLight(id);
                    break;
            }
            types.erase(id);
            refCounts.erase(id);
            freeID(id);
        }
    }

    void RenderScene::destroyTexture(const RenderObject::ID id) {
        virtualTextureStreamer.destroy(textures.at(id).getHandle());
        textures.erase(id);
    }

    void RenderScene::destroySkeleton(const RenderObject::ID id) {
        skeletonStreamer.destroy(skeletons.at(id).getBaseBone());
        skeletons.erase(id);
    }

    void RenderScene::destroyMesh(const RenderObject::ID id) {
        meshStreamer.destroy(meshes.at(id).getHandle());
        meshes.erase(id);
        skinnedMeshes.erase(id);
    }

    void RenderScene::destroyShader(const RenderObject::ID id) {
        shaders.erase(id);
    }

    void RenderScene::destroyMaterial(const RenderObject::ID id) {
        materials.erase(id);
    }

    void RenderScene::destroyModel(const RenderObject::ID id) {
        const auto &model = models.at(id);
        if (model.getMaterial().getShader().isAssigned()) {
            model.getMaterial().getShader().get().getPipeline()->removeDrawCall(model.getDrawID());
        } else {
            assert(model.getMaterial().getShadingModel() == SHADING_MODEL_PBR);
            switch (model.getMaterial().getRenderPath()) {
                case RENDER_PATH_FORWARD:
                    pbrForwardPipeline->removeDrawCall(model.getDrawID());
                    break;
                case RENDER_PATH_DEFERRED:
                    pbrDeferredPipeline->removeDrawCall(model.getDrawID());
                    break;
                default:
                    throw std::runtime_error("Unknown render path.");
            }
        }
        if (model.isCastShadows()) {
            shadowCastersPipeline->removeDrawCall(model.getShadowDrawID());
        }
        models.erase(id);
    }

    void RenderScene::destroyCanvas(const RenderObject::ID id) {
        canvases.erase(id);
    }

    void RenderScene::destroyPaint(const RenderObject::ID id) {
        const auto &paint = paints.at(id);
        paint.getCanvas().get().getPipeline()->removeDrawCall(paint.getDrawID());
        paints.erase(id);
    }

    void RenderScene::destroyPointLight(const RenderObject::ID id) {
        pointLights.erase(id);
        reuploadPointLights = true;
    }

    void RenderScene::destroyDirectionalLight(const RenderObject::ID id) {
        directionalLights.erase(id);
        reuploadDirectionalLights = true;
    }

    void RenderScene::destroySpotLight(const RenderObject::ID id) {
        spotLights.erase(id);
        reuploadSpotLights = true;
    }
}
