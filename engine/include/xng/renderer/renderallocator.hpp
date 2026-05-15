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
     * The render allocator handles allocation / streaming of persistent objects.
     */
    class RenderAllocator {
    public:
        explicit RenderAllocator(rg::Heap &heap)
            : heap(heap),
              transformStream(heap),
              boneStream(heap),
              materialStream(heap),
              pointLightStream(heap),
              spotLightStream(heap),
              directionalLightStream(heap),
              meshStream(heap),
              textureStream(heap) {
        }

        RenderAllocator(const RenderAllocator &) = delete;

        RenderAllocator &operator=(const RenderAllocator &) = delete;

        ~RenderAllocator() = default;

        RenderObjectHandle<RenderTexture> createTexture(const ImageRGBA &image) {
            return std::make_shared<RenderTexture>(textureStream, image);
        }

        RenderObjectHandle<RenderMaterial> createMaterial(const ColorRGBA &albedo,
                                                          float metallic,
                                                          float roughness,
                                                          float ambientOcclusion,
                                                          Vec4f normalIntensity,
                                                          RenderObjectHandle<RenderTexture> albedoTexture,
                                                          RenderObjectHandle<RenderTexture> metallicTexture,
                                                          RenderObjectHandle<RenderTexture> roughnessTexture,
                                                          RenderObjectHandle<RenderTexture> ambientOcclusionTexture,
                                                          RenderObjectHandle<RenderTexture> normalTexture) {
            return std::make_shared<RenderMaterial>(materialStream,
                                                    albedo,
                                                    metallic,
                                                    roughness,
                                                    ambientOcclusion,
                                                    albedoTexture,
                                                    metallicTexture,
                                                    roughnessTexture,
                                                    ambientOcclusionTexture,
                                                    normalTexture,
                                                    normalIntensity);
        }

        RenderObjectHandle<RenderSkeleton> createSkeleton(const std::vector<std::string> &boneNames) {
            return std::make_shared<RenderSkeleton>(boneStream, boneNames);
        }

        RenderObjectHandle<RenderMesh> createMesh(const Mesh &mesh, RenderObjectHandle<RenderSkeleton> skeleton) {
            return std::make_shared<RenderMesh>(meshStream, mesh, skeleton);
        }

        RenderObjectHandle<RenderModel> createModel(const std::vector<RenderObjectHandle<RenderMesh> > &meshes,
                                                    RenderObjectHandle<RenderMaterial> material,
                                                    bool receiveShadows,
                                                    bool castShadows) {
            return std::make_shared<RenderModel>(transformStream,
                                                 meshes,
                                                 material,
                                                 receiveShadows,
                                                 castShadows);
        }

        RenderObjectHandle<RenderPointLight> createPointLight() {
            return std::make_shared<RenderPointLight>(pointLightStream);
        }

        RenderObjectHandle<RenderSpotLight> createSpotLight() {
            return std::make_shared<RenderSpotLight>(spotLightStream);
        }

        RenderObjectHandle<RenderDirectionalLight> createDirectionalLight() {
            return std::make_shared<RenderDirectionalLight>(directionalLightStream);
        }

        [[nodiscard]] BufferStreamer<ShaderTransform::CPU> &getTransformStream() {
            return transformStream;
        }

        [[nodiscard]] BufferStreamer<ShaderMaterial::CPU> &getMaterialStream() {
            return materialStream;
        }

        [[nodiscard]] BufferStreamer<ShaderPointLight::CPU> &getPointLightStream() {
            return pointLightStream;
        }

        [[nodiscard]] BufferStreamer<ShaderSpotLight::CPU> &getSpotLightStream() {
            return spotLightStream;
        }

        [[nodiscard]] BufferStreamer<ShaderDirectionalLight::CPU> &getDirectionalLightStream() {
            return directionalLightStream;
        }

        [[nodiscard]] MeshStreamer &getMeshStream() {
            return meshStream;
        }

        [[nodiscard]] TextureStreamer &getTextureStream() {
            return textureStream;
        }

    private:
        rg::Heap &heap;

        BufferStreamer<ShaderTransform::CPU> transformStream;
        BufferStreamer<ShaderTransform::CPU> boneStream;
        BufferStreamer<ShaderMaterial::CPU> materialStream;

        BufferStreamer<ShaderPointLight::CPU> pointLightStream;
        BufferStreamer<ShaderSpotLight::CPU> spotLightStream;
        BufferStreamer<ShaderDirectionalLight::CPU> directionalLightStream;

        MeshStreamer meshStream;
        TextureStreamer textureStream;
    };
}

#endif //XENGINE_RENDERALLOCATOR_HPP
