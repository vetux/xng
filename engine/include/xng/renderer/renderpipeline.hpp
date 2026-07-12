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

#ifndef XENGINE_RENDERPIPELINE_HPP
#define XENGINE_RENDERPIPELINE_HPP

#include "xng/renderer/objects/renderpointlight.hpp"
#include "xng/renderer/objects/renderdirectionallight.hpp"
#include "xng/renderer/objects/renderspotlight.hpp"
#include "xng/renderer/objects/rendercanvas.hpp"
#include "xng/renderer/objects/renderfont.hpp"
#include "xng/renderer/objects/paint/renderpainttext.hpp"

#include "xng/renderer/objects/rendermodel.hpp"

#include "xng/renderer/renderpipelineshader.hpp"

namespace xng {
    /**
     * The RenderPipeline represents the rendering and allocation technique for RenderObjects.
     * This allows swapping the rendering / allocation technique at runtime (E.g. No Indirect draw on mobile platforms)
     *
     * Because the RenderObject layer is hardcoded to the set of available data and not extensible by users,
     * the renderer can abstract the rendering technique fully down to the shader level.
     *
     * This allows users to write pure shader code without having to write a custom pass. (In the future the editor will use this for the custom shading language and / or node-based shaders)
     *
     * TODO: Further RenderPipeline Design (Shader Permutations, Frames in Flight, Solve Canvas and model rendering with a unified interface / rendering path)
     */
    class RenderPipeline {
    public:
        /**
         * A draw list represents the scene split based on object properties.
         *
         * The pipeline manages persistent state per draw list such as indirect draw buffers, shader storage buffers, etc.
         *
         * The renderer manages draw lists and transforms / directs user-supplied render objects to the
         * appropriate pipeline / draw list.
         *
         * For example:
         *  DrawList pbrDeferredDrawList;
         *  DrawList pbrForwardDrawList;
         *  DrawList userShader0DrawList;
         *  DrawList userShader1DrawList;
         *  ...
         */
        class DrawList {
        public:
            virtual ~DrawList() = default;

            virtual void add(const RenderObjectHandle<RenderModel> &model) = 0;

            virtual void add(const RenderObjectHandle<RenderPointLight> &light) = 0;

            virtual void remove(RenderObject::Id id) = 0;
        };

        typedef std::variant<rg::Attachment, RenderObjectHandle<RenderTexture> > Attachment;

        struct Binding {
            rg::Resource<rg::Buffer> buffer;
            size_t offset{};
            size_t size{};
        };

        virtual ~RenderPipeline() = default;

        /**
         * Pipelines passed to draw must use the injected input / output interface.
         *
         * Users are free to do anything between input / output such as geometry stage etc. and may bind and access custom data.
         */
        virtual RenderPipelineShader &getShaderInjector() = 0;

        /**
         * Draw the specified draw list using the supplied pipeline, attachments and additional bindings.
         *
         * @param drawList The draw list to draw.
         * @param pipeline The pipeline to use for drawing.
         * @param attachments The Attachments to bind. (Type / Format must match the passed format in RenderPipelineShader::inject())
         * @param parameters Optional user-supplied shader parameters.
         * @param storageBuffers Optional user-supplied storage buffer bindings.
         * @param textureArrays Optional user-supplied texture bindings.
         * @return The Pass for performing the draw.
         */
        virtual rg::RasterPass draw(const DrawList &drawList,
                                    rg::PipelineCache::Handle pipeline,
                                    std::vector<Attachment> attachments,
                                    std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                                    std::unordered_map<std::string, Binding> storageBuffers,
                                    std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays) =0;

        virtual RenderObjectHandle<RenderTexture> createTexture(const std::shared_ptr<TileLoader> &tileLoader) = 0;

        virtual RenderObjectHandle<RenderTexture> createTexture(const ImageRGBA &image, WrappingMethod wrapping) = 0;

        virtual RenderObjectHandle<RenderMaterial> createMaterial(const ColorRGBA &albedo,
                                                                  float metallic,
                                                                  float roughness,
                                                                  float ambientOcclusion,
                                                                  float normalIntensity,
                                                                  bool flipNormal,
                                                                  RenderObjectHandle<RenderTexture> albedoTexture,
                                                                  SamplingProperties albedoProperties,
                                                                  RenderObjectHandle<RenderTexture> metallicTexture,
                                                                  SamplingProperties metallicProperties,
                                                                  RenderObjectHandle<RenderTexture> roughnessTexture,
                                                                  SamplingProperties roughnessProperties,
                                                                  RenderObjectHandle<RenderTexture>
                                                                  ambientOcclusionTexture,
                                                                  SamplingProperties ambientOcclusionProperties,
                                                                  RenderObjectHandle<RenderTexture> normalTexture,
                                                                  SamplingProperties normalProperties) = 0;

        virtual RenderObjectHandle<RenderSkeleton> createSkeleton(const std::vector<std::string> &boneNames) = 0;

        virtual RenderObjectHandle<RenderMesh> createMesh(const Mesh &mesh,
                                                          RenderObjectHandle<RenderSkeleton> skeleton) = 0;

        virtual RenderObjectHandle<RenderModel> createModel(std::vector<RenderObjectHandle<RenderMesh> > meshes,
                                                            RenderObjectHandle<RenderMaterial> material,
                                                            RenderPath renderPath,
                                                            ShadingModel shadingModel,
                                                            bool receiveShadows,
                                                            bool castShadows) = 0;

        virtual RenderObjectHandle<RenderPointLight> createPointLight() = 0;

        virtual RenderObjectHandle<RenderSpotLight> createSpotLight() = 0;

        virtual RenderObjectHandle<RenderDirectionalLight> createDirectionalLight() = 0;

        virtual RenderObjectHandle<RenderFont> createFont(std::vector<std::unique_ptr<FontRenderer> > fonts,
                                                          const Vec2i &pixelSize) = 0;

        virtual RenderObjectHandle<RenderPaintText> createPaintText(const RenderObjectHandle<RenderFont> &font,
                                                                    const std::u32string &text,
                                                                    const TextLayoutParameters &layoutParameters,
                                                                    const ColorRGBA &color,
                                                                    const SamplingProperties &sampling_properties) = 0;

        virtual RenderObjectHandle<RenderCanvas> createCanvas() = 0;

        virtual void destroy(const RenderObject &object) = 0;
    };
}

#endif //XENGINE_RENDERPIPELINE_HPP
