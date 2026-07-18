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

#include "xng/renderer/camera.hpp"

#include "xng/renderer/objects/renderskeleton.hpp"
#include "xng/renderer/objects/rendermesh.hpp"
#include "xng/renderer/objects/rendertexture.hpp"

#include "xng/renderer/objects/rendercanvas.hpp"
#include "xng/renderer/objects/renderpaint.hpp"

#include "xng/renderer/objects/renderpointlight.hpp"
#include "xng/renderer/objects/renderdirectionallight.hpp"
#include "xng/renderer/objects/renderspotlight.hpp"

#include "xng/renderer/objects/rendermodel.hpp"

#include "xng/renderer/pipeline/renderpipeline.hpp"

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
     * Multiple scenes can exist simultaneously and also be drawn simultaneously.
     * Objects cannot be shared between scenes.
     * This also allows drawing separate scenes or the same scene to separate windows.
     */
    class RenderScene {
    public:
        RenderTexture &createTexture();

        RenderSkeleton &createSkeleton();

        RenderMesh &createMesh();

        RenderModel &createModel();

        RenderPointLight &createPointLight();

        RenderDirectionalLight &createDirectionalLight();

        RenderSpotLight &createSpotLight();

        RenderCanvas &createCanvas();

        RenderPaint &createPaint();

        void destroy(RenderObject::ID id);

        void setCamera(const Camera &camera);

        // Const interface naturally defines the intent that passes do not modify the scene.

        const std::unordered_map<RenderObject::ID, RenderTexture> &getTextures() const;

        const std::unordered_map<RenderObject::ID, RenderSkeleton> &getSkeletons() const;

        const std::unordered_map<RenderObject::ID, RenderMesh> &getMeshes() const;

        const std::unordered_map<RenderObject::ID, RenderModel> &getModels() const;

        const std::unordered_map<RenderObject::ID, RenderPointLight> &getPointLights() const;

        const std::unordered_map<RenderObject::ID, RenderDirectionalLight> &getDirectionalLights() const;

        const std::unordered_map<RenderObject::ID, RenderSpotLight> &getSpotLights() const;

        const std::unordered_map<RenderObject::ID, RenderCanvas> &getCanvases() const;

        const std::unordered_map<RenderObject::ID, RenderPaint> &getPaints() const;

        void drawPbrDeferred(rg::GraphBuilder &graph,
                             const RenderShader &shader,
                             std::vector<RenderPipeline::Attachment> attachments,
                             std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                             std::unordered_map<std::string, RenderPipeline::BufferBinding> storageBuffers,
                             std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays) const;

        void drawPbrForward(rg::GraphBuilder &graph,
                            const RenderShader &shader,
                            std::vector<RenderPipeline::Attachment> attachments,
                            std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                            std::unordered_map<std::string, RenderPipeline::BufferBinding> storageBuffers,
                            std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays) const;

        void drawShadowCasters(rg::GraphBuilder &graph,
                               const RenderShader &shader,
                               std::vector<RenderPipeline::Attachment> attachments,
                               std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                               std::unordered_map<std::string, RenderPipeline::BufferBinding> storageBuffers,
                               std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays) const;

        void drawUserShading(rg::GraphBuilder &graph,
                             std::vector<RenderPipeline::Attachment> attachments,
                             std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                             std::unordered_map<std::string, RenderPipeline::BufferBinding> storageBuffers,
                             std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays) const;

        void drawScreenCanvas(rg::GraphBuilder &graph,
                              const RenderShader &shader,
                              std::vector<RenderPipeline::Attachment> attachments,
                              std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                              std::unordered_map<std::string, RenderPipeline::BufferBinding> storageBuffers,
                              std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays) const;

        void drawTextureCanvases(rg::GraphBuilder &graph,
                                 const RenderShader &shader,
                                 std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                                 std::unordered_map<std::string, RenderPipeline::BufferBinding> storageBuffers,
                                 std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays) const;

    private:
        ChunkStreamer chunkStreamer;
        SkeletonStreamer skeletonStreamer;
        MeshStreamer meshStreamer;
        VirtualTextureStreamer virtualTextureStreamer;

        // TODO: Contiguous objects storage with separate ID mapping
        std::unordered_map<RenderObject::ID, RenderTexture> textures;
        std::unordered_map<RenderObject::ID, RenderSkeleton> skeletons;
        std::unordered_map<RenderObject::ID, RenderMesh> meshes;

        std::unordered_map<RenderObject::ID, RenderModel> models;

        std::unordered_map<RenderObject::ID, RenderPointLight> pointLights;
        std::unordered_map<RenderObject::ID, RenderDirectionalLight> directionalLights;
        std::unordered_map<RenderObject::ID, RenderSpotLight> spotLights;

        std::unordered_map<RenderObject::ID, RenderCanvas> canvases;
        std::unordered_map<RenderObject::ID, RenderPaint> paints;

        RenderMesh unitQuadMesh;
        RenderMesh unitCubeMesh;

        std::unique_ptr<RenderPipeline> pbrDeferredPipeline;
        std::unique_ptr<RenderPipeline> pbrForwardPipeline;

        std::unique_ptr<RenderPipeline> shadowCastersPipeline;

        // For each user shader one pipeline with the corresponding models allocated into it.
        std::unordered_map<RenderObject::ID, std::unique_ptr<RenderPipeline> > userShadingPipelines;

        std::unique_ptr<RenderPipeline> screenCanvasPipeline;

        // For each canvas object one pipeline with models / paints allocated into it.
        std::unordered_map<RenderObject::ID, std::unique_ptr<RenderPipeline> > textureCanvasesPipelines;
    };
}

#endif //XENGINE_RENDERSCENE_HPP
