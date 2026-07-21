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

#ifndef XENGINE_RENDERSCENE_HPP
#define XENGINE_RENDERSCENE_HPP

#include "xng/renderer/materials/pbrmaterial.hpp"
#include "xng/renderer/materials/canvasmaterial.hpp"
#include "xng/renderer/camera.hpp"

#include "xng/renderer/objects/renderskeleton.hpp"
#include "xng/renderer/objects/rendermesh.hpp"
#include "xng/renderer/objects/rendertexture.hpp"
#include "xng/renderer/objects/rendermaterial.hpp"
#include "xng/renderer/objects/rendershader.hpp"

#include "xng/renderer/objects/rendermodel.hpp"

#include "xng/renderer/objects/rendercanvas.hpp"
#include "xng/renderer/objects/renderpaint.hpp"

#include "xng/renderer/objects/renderpointlight.hpp"
#include "xng/renderer/objects/renderdirectionallight.hpp"
#include "xng/renderer/objects/renderspotlight.hpp"

#include "xng/renderer/pipeline/renderpipeline.hpp"

#include "xng/renderer/renderpath.hpp"
#include "xng/renderer/shadingmodel.hpp"

namespace xng {
    /**
     * A scene represents the complete user-controllable allocation state and frame contents.
     *
     * It is bounded by the available feature set for the renderer and not directly extensible.
     * However, injection points for users are provided where possible to allow users to extend renderer functionality.
     * One of these injection points would, for example, be the user-defined materials / shaders.
     *
     * Outside of this, users can always write custom passes if certain special features require it. (Custom Post Processing etc.)
     *
     * Each scene instance owns its own set of streamers and pipelines.
     *
     * Users should be able to define default allocation sizes for the streamers when creating a scene.
     *
     * Because the renderer supports fully dynamic buffer sizes at the cost of resize costs, this allows
     * users to minimize reallocations by pre-sizing all the required buffers upfront when possible.
     *
     * In general, users would only have one scene for all level content and dynamically create / destroy objects in the scene as needed.
     * The scene should expose methods for checking streaming status for scene contents.
     *
     * Multiple scenes can be used for something like a loading screen or main menu.
     *
     * Multiple scenes can exist simultaneously.
     * Objects cannot be shared between scenes.
     * This also allows drawing separate scenes or the same scene to separate windows.
     */
    class XENGINE_EXPORT RenderScene final : public RenderObjectRefCounter {
    public:
        RenderScene(rg::Runtime &runtime,
                    ChunkStreamer &chunkStreamer,
                    size_t tileSize,
                    size_t tileBorder,
                    float maxAnisotropy);

        void setCamera(const Camera &value);

        const Camera &getCamera() const;

        RenderObjectHandle<RenderTexture> createTexture(const ImageRGBA &image,
                                                        WrappingMethod wrapping,
                                                        unsigned int mipLevels);

        RenderObjectHandle<RenderTexture> createTexture(const ImageRGBA &image, WrappingMethod wrapping);

        RenderObjectHandle<RenderTexture> createTexture(const std::shared_ptr<TileLoader> &tileLoader);

        RenderObjectHandle<RenderSkeleton> createSkeleton(const std::vector<std::string> &boneNames);

        RenderObjectHandle<RenderMesh> createMesh(const Mesh &mesh);

        RenderObjectHandle<RenderMesh> createMesh(const Mesh &mesh, RenderObjectHandle<RenderSkeleton> skeleton);

        /**
         * Create a user-defined shader.
         * Each shader owns their own pipeline.
         *
         * @param uShaders
         * @param materialLayout
         * @param pipelineConfiguration
         * @return
         */
        RenderObjectHandle<RenderShader> createShader(const std::vector<rg::Shader> &uShaders,
                                                      RenderPipeline::MaterialLayout materialLayout,
                                                      const rg::RasterPipeline::Configuration &pipelineConfiguration);

        /**
         * Create a user-defined material for the given shader.
         *
         * @param shader
         * @return
         */
        RenderObjectHandle<RenderMaterial> createMaterial(const RenderObjectHandle<RenderShader> &shader);

        /**
         * Create a PBR material.
         *
         * @param material The material data.
         * @param renderPath The render path for this material.
         */
        RenderObjectHandle<RenderMaterial> createMaterial(const PBRMaterial &material, RenderPath renderPath);

        /**
         * Create a model.
         *
         * @param material
         * @param mMeshes
         * @param castShadows
         * @param sortPriority
         * @return
         */
        RenderObjectHandle<RenderModel> createModel(const RenderObjectHandle<RenderMaterial> &material,
                                                    const std::vector<RenderObjectHandle<RenderMesh> > &mMeshes,
                                                    bool castShadows,
                                                    int sortPriority);

        /**
         * Create a screen space canvas.
         *
         * @return
         */
        RenderObjectHandle<RenderCanvas> createCanvas();

        /**
         * Create a texture canvas.
         *
         * The canvas will be rendered to the given texture.
         *
         * The texture can be used as a material texture for models which allows
         * for a flexible implementation of world space canvases. (Custom shading, Non-Planar canvas mesh, etc.)
         *
         * @param texture
         * @return
         */
        RenderObjectHandle<RenderCanvas> createCanvas(const RenderObjectHandle<RenderTexture> &texture);

        /**
         * Create a line paint object.
         *
         * @param canvas
         * @param start
         * @param end
         * @param color
         * @param center
         * @param rotation
         * @param sortPriority
         * @return
         */
        RenderObjectHandle<RenderPaint> createPaint(const RenderObjectHandle<RenderCanvas> &canvas,
                                                    const Vec2f &start,
                                                    const Vec2f &end,
                                                    const ColorRGBA &color,
                                                    const Vec2f &center = {},
                                                    float rotation = 0,
                                                    int sortPriority = 0);

        /**
         * Create a point paint object.
         *
         * @param canvas
         * @param position
         * @param size
         * @param color
         * @param sortPriority
         * @return
         */
        RenderObjectHandle<RenderPaint> createPaint(const RenderObjectHandle<RenderCanvas> &canvas,
                                                    const Vec2f &position,
                                                    float size,
                                                    const ColorRGBA &color,
                                                    int sortPriority = 0);

        /**
         * Create a rectangle paint object.
         *
         * @param canvas
         * @param dstRect
         * @param color
         * @param center
         * @param rotation
         * @param sortPriority
         * @return
         */
        RenderObjectHandle<RenderPaint> createPaint(const RenderObjectHandle<RenderCanvas> &canvas,
                                                    const Rectf &dstRect,
                                                    const ColorRGBA &color,
                                                    const Vec2f &center = {},
                                                    float rotation = 0,
                                                    int sortPriority = 0);

        /**
         * Create a texture paint object.
         *
         * @param canvas
         * @param dstRect
         * @param texture
         * @param samplingProperties
         * @param mixColor The color to mix with the sampled texture color.
         * @param mix The mixing factor for each color channel.
         * @param center
         * @param rotation
         * @param sortPriority
         * @return
         */
        RenderObjectHandle<RenderPaint> createPaint(const RenderObjectHandle<RenderCanvas> &canvas,
                                                    const Rectf &dstRect,
                                                    const RenderObjectHandle<RenderTexture> &texture,
                                                    const SamplingProperties &samplingProperties,
                                                    const ColorRGBA &mixColor,
                                                    const Vec4f &mix,
                                                    const Vec2f &center = {},
                                                    float rotation = 0,
                                                    int sortPriority = 0);

        RenderObjectHandle<RenderPointLight> createPointLight();

        RenderObjectHandle<RenderDirectionalLight> createDirectionalLight();

        RenderObjectHandle<RenderSpotLight> createSpotLight();

        void commit(rg::GraphBuilder &graph, StreamerQueue &streamerQueue);

        void prepare(rg::GraphBuilder &graph);

        const RenderPipeline &getPbrDeferredPipeline() const {
            return *pbrDeferredPipeline;
        }

        const RenderPipeline &getPbrForwardPipeline() const {
            return *pbrForwardPipeline;
        }

        const RenderPipeline &getShadowCastersPipeline() const {
            return *shadowCastersPipeline;
        }

        const std::unordered_map<RenderObject::ID, RenderTexture> &getTextures() const {
            return textures;
        }

        const std::unordered_map<RenderObject::ID, RenderSkeleton> &getSkeletons() const {
            return skeletons;
        }

        const std::unordered_map<RenderObject::ID, RenderMesh> &getMeshes() const {
            return meshes;
        }

        const std::unordered_map<RenderObject::ID, RenderShader> &getShaders() const {
            return shaders;
        }

        const std::unordered_map<RenderObject::ID, RenderMaterial> &getMaterials() const {
            return materials;
        }

        const std::unordered_map<RenderObject::ID, RenderModel> &getModels() const {
            return models;
        }

        const std::unordered_map<RenderObject::ID, RenderCanvas> &getCanvases() const {
            return canvases;
        }

        const std::unordered_map<RenderObject::ID, RenderPaint> &getPaints() const {
            return paints;
        }

        const std::unordered_map<RenderObject::ID, RenderPointLight> &getPointLights() const {
            return pointLights;
        }

        const std::unordered_map<RenderObject::ID, RenderDirectionalLight> &getDirectionalLights() const {
            return directionalLights;
        }

        const std::unordered_map<RenderObject::ID, RenderSpotLight> &getSpotLights() const {
            return spotLights;
        }

        const VirtualTextureStreamer &getVirtualTextureStreamer() const {
            return virtualTextureStreamer;
        }

        const SkeletonStreamer &getSkeletonStreamer() const {
            return skeletonStreamer;
        }

        const MeshStreamer &getMeshStreamer() const {
            return meshStreamer;
        }

        rg::HeapResource<rg::Buffer> getPointLightBuffer() const {
            return pointLightBuffer.getBuffer();
        }

        rg::HeapResource<rg::Buffer> getDirectionalLightBuffer() const {
            return directionalLightBuffer.getBuffer();
        }

        rg::HeapResource<rg::Buffer> getSpotLightBuffer() const {
            return spotLightBuffer.getBuffer();
        }

        const RenderObjectHandle<RenderMesh> &getUnitQuadMesh() const {
            return unitQuadMesh;
        }

        const RenderObjectHandle<RenderMesh> &getUnitCubeMesh() const {
            return unitCubeMesh;
        }

        const std::unordered_set<RenderObject::ID> &getSkinnedMeshes() const {
            return skinnedMeshes;
        }

    private:
        std::shared_ptr<RenderPipeline> createPipeline(RenderPipeline::MaterialLayout materialLayout);

        void incrementReference(RenderObject::ID id) override;

        void decrementReference(RenderObject::ID id) override;

        void destroyTexture(RenderObject::ID id);

        void destroySkeleton(RenderObject::ID id);

        void destroyMesh(RenderObject::ID id);

        void destroyShader(RenderObject::ID id);

        void destroyMaterial(RenderObject::ID id);

        void destroyModel(RenderObject::ID id);

        void destroyCanvas(RenderObject::ID id);

        void destroyPaint(RenderObject::ID id);

        void destroyPointLight(RenderObject::ID id);

        void destroyDirectionalLight(RenderObject::ID id);

        void destroySpotLight(RenderObject::ID id);

        RenderObject::ID allocateID() {
            if (freeIDs.empty()) {
                return nextID++;
            }

            const auto ret = freeIDs.back();
            freeIDs.pop_back();
            return ret;
        }

        void freeID(const RenderObject::ID id) {
            freeIDs.push_back(id);
        }

        RenderObject::ID nextID = RenderObject::UNASSIGNED_ID + 1;
        std::vector<RenderObject::ID> freeIDs;

        rg::Runtime &runtime;

        ChunkStreamer &chunkStreamer;

        SkeletonStreamer skeletonStreamer;
        MeshStreamer meshStreamer;
        VirtualTextureStreamer virtualTextureStreamer;

        StreamBuffer pointLightBuffer;
        StreamBuffer directionalLightBuffer;
        StreamBuffer spotLightBuffer;

        StreamBuffer::Handle pointLightBufferHandle;
        StreamBuffer::Handle directionalLightBufferHandle;
        StreamBuffer::Handle spotLightBufferHandle;

        bool pointLightResident = false;
        bool directionalLightResident = false;
        bool spotLightResident = false;

        bool reuploadPointLights = false;
        bool reuploadDirectionalLights = false;
        bool reuploadSpotLights = false;

        Camera camera;

        // TODO: Contiguous objects storage with separate ID mapping
        std::unordered_map<RenderObject::ID, RenderTexture> textures;
        std::unordered_map<RenderObject::ID, RenderSkeleton> skeletons;
        std::unordered_map<RenderObject::ID, RenderMesh> meshes;
        std::unordered_map<RenderObject::ID, RenderShader> shaders;
        std::unordered_map<RenderObject::ID, RenderMaterial> materials;

        std::unordered_map<RenderObject::ID, RenderModel> models;

        std::unordered_map<RenderObject::ID, RenderCanvas> canvases;
        std::unordered_map<RenderObject::ID, RenderPaint> paints;

        std::unordered_map<RenderObject::ID, RenderPointLight> pointLights;
        std::unordered_map<RenderObject::ID, RenderDirectionalLight> directionalLights;
        std::unordered_map<RenderObject::ID, RenderSpotLight> spotLights;

        std::unordered_map<RenderObject::ID, size_t> refCounts;
        std::unordered_map<RenderObject::ID, RenderObject::Type> types;

        std::unordered_set<RenderObject::ID> skinnedMeshes;

        RenderObjectHandle<RenderMesh> unitQuadMesh;
        RenderObjectHandle<RenderMesh> unitCubeMesh;

        std::unique_ptr<RenderPipeline> pbrDeferredPipeline;
        std::unique_ptr<RenderPipeline> pbrForwardPipeline;
        std::unique_ptr<RenderPipeline> shadowCastersPipeline;
    };
}

#endif //XENGINE_RENDERSCENE_HPP
