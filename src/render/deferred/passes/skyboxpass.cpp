/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "render/deferred/passes/skyboxpass.hpp"
#include "render/deferred/deferredpipeline.hpp"
#include "render/shader/shaderinclude.hpp"

#include "asset/assetimporter.hpp"

#include <sstream>

static const char *SHADER_VERT = R"###(
struct VS_INPUT
{
    float3 position : POSITION0;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent: TANGENT;
    float3 bitangent: BINORMAL;
    float4 instanceRow0 : POSITION1;
    float4 instanceRow1 : POSITION2;
    float4 instanceRow2 : POSITION3;
    float4 instanceRow3 : POSITION4;
};

struct VS_OUTPUT
{
    float3  fPos : POSITION0;
    float3  fNorm : NORMAL;
    float2  fUv : TEXCOORD0;
    float4 vPos : SV_Position;
    float3 worldPos : POSITION1;
};

float4x4 MANA_VIEW_TRANSLATION;
float4x4 MANA_M;
float4x4 MANA_V;
float4x4 MANA_P;

VS_OUTPUT main(const VS_INPUT v)
{
    VS_OUTPUT ret;

    float4x4 t = mul(MANA_VIEW_TRANSLATION, mul(MANA_V, MANA_P));

    ret.vPos = mul(float4(v.position, 1), t);
    ret.fPos = mul(float4(v.position, 1), t).xyz;
    ret.worldPos = v.position;
    ret.fNorm = mul(v.normal, MANA_M).xyz;
    ret.fUv = v.uv;

    return ret;
}
)###";

static const char *SHADER_FRAG = R"###(

struct PS_INPUT {
    float3 fPos: POSITION0;
    float3 fNorm: NORMAL;
    float2 fUv: TEXCOORD0;
    float3 worldPos : POSITION1;
};

struct PS_OUTPUT {
     float4 color     :   SV_TARGET0;
};

TextureCube diffuse;

SamplerState samplerState_diffuse
{};

PS_OUTPUT main(PS_INPUT v) {
    PS_OUTPUT ret;
    ret.color = diffuse.Sample(samplerState_diffuse, v.worldPos);
    return ret;
}
)###";

namespace xengine {
    SkyboxPass::SkyboxPass(RenderDevice &device)
            : device(device) {
        vert = ShaderSource(SHADER_VERT, "main", VERTEX, HLSL_SHADER_MODEL_4);
        frag = ShaderSource(SHADER_FRAG, "main", FRAGMENT, HLSL_SHADER_MODEL_4);

        vert.preprocess(ShaderInclude::getShaderIncludeCallback(),
                        ShaderInclude::getShaderMacros(HLSL_SHADER_MODEL_4));
        frag.preprocess(ShaderInclude::getShaderIncludeCallback(),
                        ShaderInclude::getShaderMacros(HLSL_SHADER_MODEL_4));

        auto &allocator = device.getAllocator();

        shader = allocator.createShaderProgram(vert, frag);

        TextureBuffer::Attributes attributes;
        attributes.size = Vec2i(1, 1);
        attributes.format = TextureBuffer::RGBA;
        attributes.textureType = TextureBuffer::TEXTURE_CUBE_MAP;
        attributes.generateMipmap = false;
        attributes.wrapping = TextureBuffer::REPEAT;

        defaultTexture = allocator.createTextureBuffer(attributes);

        meshBuffer = allocator.createMeshBuffer(Mesh::normalizedCube());

        resizeTextureBuffers({1, 1}, device.getAllocator(), false);
    }

    void SkyboxPass::render(GBuffer &gBuffer, Scene &scene, AssetRenderManager &assetRenderManager) {
        auto &ren = device.getRenderer();

        shader->activate();
        shader->setTexture("diffuse", 0);

        Mat4f model, view, projection, cameraTranslation;
        view = scene.camera.view();
        projection = scene.camera.projection();
        cameraTranslation = MatrixMath::translate(scene.camera.transform.getPosition());

        //Draw skybox
        auto &target = gBuffer.getPassTarget();

        if (colorBuffer->getAttributes().size != gBuffer.getSize()) {
            resizeTextureBuffers(gBuffer.getSize(), device.getAllocator(), false);
        }

        target.setNumberOfColorAttachments(1);
        target.attachColor(0, *colorBuffer);

        ren.renderBegin(target, RenderOptions({}, target.getSize(), false));

        shader->setMat4("MANA_M", model);
        shader->setMat4("MANA_V", view);
        shader->setMat4("MANA_P", projection);
        shader->setMat4("MANA_MVP", projection * view * model);
        shader->setMat4("MANA_M_INVERT", MatrixMath::inverse(model));
        shader->setMat4("MANA_VIEW_TRANSLATION", cameraTranslation);

        RenderCommand skyboxCommand(*shader, *meshBuffer);

        if (scene.skybox.texture.empty()) {
            for (int i = TextureBuffer::CubeMapFace::POSITIVE_X; i <= TextureBuffer::CubeMapFace::NEGATIVE_Z; i++) {
                defaultTexture->upload(static_cast<TextureBuffer::CubeMapFace>(i),
                                       Image<ColorRGBA>(1, 1, {scene.skybox.color}));
            }
            skyboxCommand.textures.emplace_back(*defaultTexture);
        } else {
            skyboxCommand.textures.emplace_back(assetRenderManager.get<TextureBuffer>(scene.skybox.texture));
        }

        skyboxCommand.properties.enableDepthTest = false;
        skyboxCommand.properties.enableFaceCulling = false;

        ren.addCommand(skyboxCommand);

        ren.renderFinish();

        target.detachColor(0);
    }
}