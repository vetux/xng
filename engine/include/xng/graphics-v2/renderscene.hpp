/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_RENDERSCENE_HPP
#define XENGINE_RENDERSCENE_HPP

#include "xng/assets/assetscene.hpp"

#include "xng/graphics-v2/objects/rendercamera.hpp"
#include "xng/graphics-v2/objects/renderdirectionallight.hpp"
#include "xng/graphics-v2/objects/rendermaterial.hpp"
#include "xng/graphics-v2/objects/rendermesh.hpp"
#include "xng/graphics-v2/objects/rendermodel.hpp"
#include "xng/graphics-v2/objects/renderpointlight.hpp"
#include "xng/graphics-v2/objects/renderskeleton.hpp"
#include "xng/graphics-v2/objects/renderspotlight.hpp"
#include "xng/graphics-v2/objects/rendertexture.hpp"

namespace xng {
    /**
     * The render scene represents the scene allocation state of the renderer.
     *
     * The scene objects directly correlate to heap resources. This enables fine-grained gpu state updates.
     *
     * The scene object data is streamed to the heap asynchronously.
     *
     * All passes now share a common heap resource pool and only handle transient allocations.
     *
     * It contains ALL heap allocations for the passes.
     */
    class RenderScene {
    public:
        explicit RenderScene(rg::Heap &heap);

        ~RenderScene();

        RenderObjectHandle<RenderCamera> createCamera();

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
        rg::Heap &heap;

        BufferStreamer<ShaderCamera::CPU> cameraStream;
        BufferStreamer<ShaderTransform::CPU> transformStream;
        BufferStreamer<ShaderTransform::CPU> boneStream;
        MeshStreamer meshStream;
        BufferStreamer<ShaderMaterial::CPU> materialStream;
        TextureStreamer textureStream;

        BufferStreamer<ShaderPointLight::CPU> pointLightStream;
        BufferStreamer<ShaderSpotLight::CPU> spotLightStream;
        BufferStreamer<ShaderDirectionalLight::CPU> directionalLightStream;

        StreamTexture pointShadowMaps;
        StreamTexture spotShadowMaps;
        StreamTexture directionalShadowMaps;
    };
}

#endif //XENGINE_RENDERSCENE_HPP
