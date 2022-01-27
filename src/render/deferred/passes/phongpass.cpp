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

#include "platform/graphics/shadercompiler.hpp"
#include "render/shader/shaderinclude.hpp"
#include "render/deferred/passes/phongpass.hpp"
#include "render/deferred/deferredpipeline.hpp"
#include "math/rotation.hpp"
#include "async/threadpool.hpp"

//TODO: Fix phong pass at random times consistently outputting black color for non normal mapped objects.
const char *SHADER_VERT_LIGHTING = R"###(
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
    float3  fPos : POSITION;
    float3  fNorm : NORMAL;
    float2  fUv : TEXCOORD0;
    float4 vPos : SV_Position;
};

VS_OUTPUT main(const VS_INPUT v)
{
    VS_OUTPUT ret;

    ret.fPos = v.position;
    ret.fNorm = v.normal;
    ret.fUv = v.uv;

    ret.vPos = float4(v.position, 1);

    return ret;
}
)###";

const char *SHADER_FRAG_LIGHTING = R"###(
#include "phong.hlsl"

struct PS_INPUT {
    float3 fPos: POSITION;
    float3 fNorm: NORMAL;
    float2 fUv: TEXCOORD0;
};

struct PS_OUTPUT {
    float4 color: SV_TARGET0;
    float depth : SV_Depth;
};

Texture2DMS<float4> position;
Texture2DMS<float4> normal;
Texture2DMS<float4> tangent;
Texture2DMS<float4> texNormal;
Texture2DMS<float4> diffuse;
Texture2DMS<float4> ambient;
Texture2DMS<float4> specular;
Texture2DMS<float4> shininess;
Texture2DMS<float4> depth;

float3 VIEW_POS;

LightComponents getSampleLightComponents(int2 coord, int sampleIndex)
{
    float3 fragPosition = position.Load(coord, sampleIndex).xyz;
    float3 fragNormal = normal.Load(coord, sampleIndex).xyz;
    float3 fragTangent = tangent.Load(coord, sampleIndex).xyz;
    float4 fragDiffuse = diffuse.Load(coord, sampleIndex);
    float4 fragSpecular = specular.Load(coord, sampleIndex);
    float fragShininess = shininess.Load(coord, sampleIndex).x;
    float3 fragTexNormal = texNormal.Load(coord, sampleIndex).xyz;

    if (length(fragTexNormal) > 0)
    {
        // Calculate lighting in tangent space, does not output correct value
        float3x3 TBN = transpose(float3x3(cross(fragNormal, normalize(fragTangent - dot(fragTangent, fragNormal) * fragNormal)), fragTangent, fragNormal));

        return mana_calculate_light(fragPosition * TBN,
                                    fragTexNormal,
                                    fragDiffuse,
                                    fragSpecular,
                                    fragShininess,
                                    VIEW_POS * TBN,
                                    TBN);
    }
    else
    {
        // Calculate lighting in world space
        return mana_calculate_light(fragPosition,
                                fragNormal,
                                fragDiffuse,
                                fragSpecular,
                                fragShininess,
                                VIEW_POS,
                                float3x3(1));
    }
}

PS_OUTPUT main(PS_INPUT v, uint sampleIndex : SV_SampleIndex) {
    PS_OUTPUT ret;

    uint2 size;
    int samples;
    position.GetDimensions(size.x, size.y, samples);
    int2 coord = v.fUv * size;

    LightComponents comp = getSampleLightComponents(coord, sampleIndex);

    float3 comb = comp.ambient + comp.diffuse + comp.specular;

    ret.depth = depth.Load(coord, sampleIndex);
    if (ret.depth < 1)
        ret.color = float4(comb.r, comb.g, comb.b, 1);

    return ret;
}
)###";

namespace xengine {
    using namespace ShaderCompiler;

    PhongPass::PhongPass(RenderDevice &device)
            : renderDevice(device) {
        vertexShader = ShaderSource(SHADER_VERT_LIGHTING,
                                    "main",
                                    VERTEX,
                                    HLSL_SHADER_MODEL_4);
        fragmentShader = ShaderSource(SHADER_FRAG_LIGHTING,
                                      "main",
                                      FRAGMENT,
                                      HLSL_SHADER_MODEL_4);

        vertexShader.preprocess(ShaderInclude::getShaderIncludeCallback(),
                                ShaderInclude::getShaderMacros(HLSL_SHADER_MODEL_4));
        fragmentShader.preprocess(ShaderInclude::getShaderIncludeCallback(),
                                  ShaderInclude::getShaderMacros(HLSL_SHADER_MODEL_4));

        auto &allocator = device.getAllocator();

        shader = allocator.createShaderProgram(vertexShader, fragmentShader);

        multiSampleTarget = allocator.createRenderTarget({1, 1}, 1);

        resizeTextureBuffers({1, 1}, device.getAllocator(), true);
    }

    void PhongPass::render(GBuffer &gBuffer, Scene &scene, AssetRenderManager &assetRenderManager) {
        if (colorBuffer->getAttributes().size != gBuffer.getSize() ||
            multiSampleTarget->getSamples() != gBuffer.getSamples()) {
            resizeTextureBuffers(gBuffer.getSize(), renderDevice.getAllocator(), true);

            auto &allocator = renderDevice.getAllocator();

            multiSampleTarget = allocator.createRenderTarget(gBuffer.getSize(), gBuffer.getSamples());
        }

        int dirCount = 0;
        int pointCount = 0;
        int spotCount = 0;

        shader->activate();

        for (auto &light: scene.lights) {
            std::string name;
            switch (light.type) {
                case LIGHT_DIRECTIONAL:
                    name = "DIRECTIONAL_LIGHTS[" + std::to_string(dirCount++) + "].";
                    shader->setVec3(name + "direction", light.direction);
                    shader->setVec3(name + "ambient", light.ambient);
                    shader->setVec3(name + "diffuse", light.diffuse);
                    shader->setVec3(name + "specular", light.specular);
                    break;
                case LIGHT_POINT:
                    name = "POINT_LIGHTS[" + std::to_string(pointCount++) + "].";
                    shader->setVec3(name + "position", light.transform.getPosition());
                    shader->setFloat(name + "constantValue", light.constant);
                    shader->setFloat(name + "linearValue", light.linear);
                    shader->setFloat(name + "quadraticValue", light.quadratic);
                    shader->setVec3(name + "ambient", light.ambient);
                    shader->setVec3(name + "diffuse", light.diffuse);
                    shader->setVec3(name + "specular", light.specular);
                    break;
                case LIGHT_SPOT:
                    name = "SPOT_LIGHTS[" + std::to_string(spotCount++) + "].";
                    shader->setVec3(name + "position", light.transform.getPosition());
                    shader->setVec3(name + "direction", light.direction);
                    shader->setFloat(name + "cutOff", cosf(degreesToRadians(light.cutOff)));
                    shader->setFloat(name + "outerCutOff", cosf(degreesToRadians(light.outerCutOff)));
                    shader->setFloat(name + "constantValue", light.constant);
                    shader->setFloat(name + "linearValue", light.linear);
                    shader->setFloat(name + "quadraticValue", light.quadratic);
                    shader->setVec3(name + "ambient", light.ambient);
                    shader->setVec3(name + "diffuse", light.diffuse);
                    shader->setVec3(name + "specular", light.specular);
                    break;
            }
        }

        shader->setInt("DIRECTIONAL_LIGHTS_COUNT", dirCount);
        shader->setInt("POINT_LIGHTS_COUNT", pointCount);
        shader->setInt("SPOT_LIGHTS_COUNT", spotCount);

        shader->setTexture("position", 0);
        shader->setTexture("normal", 1);
        shader->setTexture("tangent", 2);
        shader->setTexture("texNormal", 3);
        shader->setTexture("diffuse", 4);
        shader->setTexture("ambient", 5);
        shader->setTexture("specular", 6);
        shader->setTexture("shininess", 7);
        shader->setTexture("depth", 8);

        shader->setVec3("VIEW_POS", scene.camera.transform.getPosition());

        RenderCommand command(*shader, gBuffer.getScreenQuad());

        command.textures.emplace_back(gBuffer.getTexture(GBuffer::POSITION));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::NORMAL));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::TANGENT));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::TEXTURE_NORMAL));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::DIFFUSE));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::AMBIENT));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::SPECULAR));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::ID_SHININESS));
        command.textures.emplace_back(gBuffer.getTexture(GBuffer::DEPTH));

        command.properties.enableDepthTest = false;
        command.properties.enableStencilTest = false;
        command.properties.enableFaceCulling = false;
        command.properties.enableBlending = false;

        auto &ren = renderDevice.getRenderer();

        auto &target = gBuffer.getPassTarget();

        target.setNumberOfColorAttachments(1);
        target.attachColor(0, *colorBuffer);
        target.attachDepthStencil(*depthBuffer);

        ren.renderClear(target, {}, 1);

        ren.renderBegin(*multiSampleTarget, RenderOptions({}, gBuffer.getSize(), true));
        ren.addCommand(command);
        ren.renderFinish();

        target.blitColor(*multiSampleTarget, {}, {}, multiSampleTarget->getSize(), target.getSize(),
                         TextureBuffer::LINEAR, 0, 0);
        target.blitDepth(*multiSampleTarget, {}, {}, multiSampleTarget->getSize(), target.getSize());

        target.detachColor(0);
        target.detachDepthStencil();
    }
}