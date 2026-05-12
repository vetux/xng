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

#ifndef XENGINE_RENDERALLOCATOR_HPP
#define XENGINE_RENDERALLOCATOR_HPP

#include "xng/assets/assetscene.hpp"

#include "xng/renderer/camera.hpp"
#include "xng/renderer/renderscene.hpp"
#include "xng/renderer/objects/renderdirectionallight.hpp"
#include "xng/renderer/objects/rendermaterial.hpp"
#include "xng/renderer/objects/rendermesh.hpp"
#include "xng/renderer/objects/rendermodel.hpp"
#include "xng/renderer/objects/renderpointlight.hpp"
#include "xng/renderer/objects/renderskeleton.hpp"
#include "xng/renderer/objects/renderspotlight.hpp"
#include "xng/renderer/objects/rendertexture.hpp"

namespace xng {
    /**
     * The render allocator handles allocation / streaming of objects.
     */
    class RenderAllocator {
    public:
        explicit RenderAllocator(rg::Heap &heap);

        RenderAllocator(const RenderAllocator &) = delete;

        RenderAllocator &operator=(const RenderAllocator &) = delete;

        ~RenderAllocator();

        /**
         * Currently, only models can be selectively drawn in a scene.
         * All existing lights will be present in the returned scene.
         *
         * More sophisticated packing strategies can be implemented later on based on referenced objects.
         * Before this user-controlled heap resource address support should be implemented in the render graph to really get 100% cache locality.
         *
         * @param builder The builder to use for committing the streams.
         * @param models The models to draw
         * @return The scene
         */
        RenderScene createScene(rg::GraphBuilder &builder, const std::vector<RenderObjectHandle<RenderModel> > &models);

        void setCamera(const Camera &camera);

        RenderObjectHandle<RenderTexture> createTexture(const ImageRGBA &image);

        RenderObjectHandle<RenderMaterial> createMaterial(const ColorRGBA &albedo,
                                                          float metallic,
                                                          float roughness,
                                                          float ambientOcclusion,
                                                          Vec4f normalIntensity,
                                                          RenderObjectHandle<RenderTexture> albedoTexture,
                                                          RenderObjectHandle<RenderTexture> metallicTexture,
                                                          RenderObjectHandle<RenderTexture> roughnessTexture,
                                                          RenderObjectHandle<RenderTexture> ambientOcclusionTexture,
                                                          RenderObjectHandle<RenderTexture> normalTexture);

        RenderObjectHandle<RenderSkeleton> createSkeleton(const std::vector<std::string> &boneNames);

        RenderObjectHandle<RenderMesh> createMesh(const Mesh &mesh, RenderObjectHandle<RenderSkeleton> skeleton);

        RenderObjectHandle<RenderModel> createModel(const std::vector<RenderObjectHandle<RenderMesh> > &meshes,
                                                    RenderObjectHandle<RenderMaterial> material,
                                                    bool receiveShadows);

        RenderObjectHandle<RenderPointLight> createPointLight();

        RenderObjectHandle<RenderSpotLight> createSpotLight();

        RenderObjectHandle<RenderDirectionalLight> createDirectionalLight();

    private:
        struct Streams {
            StreamBuffer cameraStream;
            BufferStreamer<ShaderTransform::CPU> transformStream;
            BufferStreamer<ShaderTransform::CPU> boneStream;
            BufferStreamer<ShaderMaterial::CPU> materialStream;

            BufferStreamer<ShaderPointLight::CPU> pointLightStream;
            BufferStreamer<ShaderSpotLight::CPU> spotLightStream;
            BufferStreamer<ShaderDirectionalLight::CPU> directionalLightStream;

            StreamTexture pointShadowMaps;
            StreamTexture spotShadowMaps;
            StreamTexture directionalShadowMaps;

            MeshStreamer meshStream;
            TextureStreamer textureStream;

            explicit Streams(rg::Heap &heap, const Vec2i &shadowMapResolution)
                : cameraStream(heap, rg::Buffer::CAPABILITY_STORAGE),
                  transformStream(heap),
                  boneStream(heap),
                  materialStream(heap),
                  pointLightStream(heap),
                  spotLightStream(heap),
                  directionalLightStream(heap),
                  pointShadowMaps(heap, rg::Texture(rg::Texture::CAPABILITY_DEPTH_STENCIL_ATTACHMENT
                                                    | rg::Texture::CAPABILITY_SAMPLED,
                                                    shadowMapResolution,
                                                    rg::TEXTURE_CUBE_MAP,
                                                    rg::DEPTH_32F)),
                  spotShadowMaps(heap, rg::Texture(rg::Texture::CAPABILITY_DEPTH_STENCIL_ATTACHMENT
                                                   | rg::Texture::CAPABILITY_SAMPLED,
                                                   shadowMapResolution,
                                                   rg::TEXTURE_2D,
                                                   rg::DEPTH_32F)),
                  directionalShadowMaps(heap, rg::Texture(rg::Texture::CAPABILITY_DEPTH_STENCIL_ATTACHMENT
                                                          | rg::Texture::CAPABILITY_SAMPLED,
                                                          shadowMapResolution,
                                                          rg::TEXTURE_2D,
                                                          rg::DEPTH_32F)),
                  meshStream(heap),
                  textureStream(heap) {
            }
        };

        rg::Heap &heap;
        Streams streams;
    };
}

#endif //XENGINE_RENDERALLOCATOR_HPP
