/**
 *  This file is part of xEngine, a C++ game engine library.
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

#ifndef XENGINE_GBUFFERPASS_HPP
#define XENGINE_GBUFFERPASS_HPP

#include "xng/render/graph/framegraphpass.hpp"
#include "xng/resource/uri.hpp"
#include "xng/render/scene.hpp"

namespace xng {
    /**
     * The GBufferPass creates geometry buffer textures which contain the data of the objects
     * which dont contain a user specified shader and are not marked as transparent.
     *
     * No Dependencies
     */
    class XENGINE_EXPORT GBufferPass : public FrameGraphPass {
    public:
        // FrameGraphResource to a Texture RGBA32F : World Space Position xyz, w = X
        SHARED_PROPERTY(GBufferPass, GEOMETRY_BUFFER_POSITION)

        // FrameGraphResource to a Texture RGBA32F : Vertex or Texture Normal xyz, w = X
        SHARED_PROPERTY(GBufferPass, GEOMETRY_BUFFER_NORMAL)

        // FrameGraphResource to a Texture RGBA32F : Vertex Tangent xyz, w = X
        SHARED_PROPERTY(GBufferPass, GEOMETRY_BUFFER_TANGENT)

        // FrameGraphResource to a Texture RGBA32f : .x = pbr roughness or phong shininess, .y = pbr metallic, .z = pbr ambient occlusion
        SHARED_PROPERTY(GBufferPass, GEOMETRY_BUFFER_ROUGHNESS_METALLIC_AO)

        // FrameGraphResource to a Texture RGBA : The pbr albedo or phong diffuse color value
        SHARED_PROPERTY(GBufferPass, GEOMETRY_BUFFER_ALBEDO)

        // FrameGraphResource to a Texture RGBA : The phong ambient color value
        SHARED_PROPERTY(GBufferPass, GEOMETRY_BUFFER_AMBIENT)

        // FrameGraphResource to a  Texture RGBA : The phong specular color value
        SHARED_PROPERTY(GBufferPass, GEOMETRY_BUFFER_SPECULAR)

        // FrameGraphResource to a Texture RGBA32I : .x = Shading Model ID, .y = Object ID
        SHARED_PROPERTY(GBufferPass, GEOMETRY_BUFFER_MODEL_OBJECT)

        // FrameGraphResource to a Texture DEPTH_STENCIL : The depth value in the x component
        SHARED_PROPERTY(GBufferPass, GEOMETRY_BUFFER_DEPTH)

        GBufferPass();

        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources) override;

        std::type_index getTypeIndex() override;

    private:
        FrameGraphResource renderTarget;
        FrameGraphResource phongPipeline;
        FrameGraphResource pbrPipeline;

        FrameGraphResource pbrShaderBuffer;
        FrameGraphResource phongShaderBuffer;

        FrameGraphResource defaultTexture;

        FrameGraphResource gBufferPosition;
        FrameGraphResource gBufferNormal;
        FrameGraphResource gBufferTangent;
        FrameGraphResource gBufferRoughnessMetallicAmbientOcclusion;
        FrameGraphResource gBufferAlbedo;
        FrameGraphResource gBufferAmbient;
        FrameGraphResource gBufferSpecular;
        FrameGraphResource gBufferModelObject;
        FrameGraphResource gBufferDepth;

        std::map<Uri, FrameGraphResource> meshBuffers;
        std::map<Uri, FrameGraphResource> textureBuffers;

        std::map<uint, FrameGraphResource> shaderBuffers; // The shader buffer of each object in the scene

        std::vector<std::pair<uint, Scene::Object>> phongObjects;
        std::vector<std::pair<uint, Scene::Object>> pbrObjects;

        Camera camera;
        Transform cameraTransform;
    };
}
#endif //XENGINE_GBUFFERPASS_HPP
