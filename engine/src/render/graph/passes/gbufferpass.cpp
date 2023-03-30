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

#include "xng/render/graph/passes/gbufferpass.hpp"
#include "xng/render/graph/framegraphbuilder.hpp"
#include "xng/render/graph/framegraphproperties.hpp"
#include "xng/render/textureatlas.hpp"
#include "xng/render/shaderinclude.hpp"

#include "xng/io/protocol/jsonprotocol.hpp"

#include "xng/resource/parsers/jsonparser.hpp"

#include "xng/geometry/vertexstream.hpp"

#pragma message "Not Implemented"

//TODO: Fix GBufferPass not drawing the geometry correctly (Clearing the textures is visible in subsequent passes)

static const char *SHADER_VERT_GEOMETRY = R"###(#version 460

#define MAX_MULTI_DRAW 1000

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 boneWeights;

layout(location = 0) out vec3 fPos;
layout(location = 1) out vec3 fNorm;
layout(location = 2) out vec3 fTan;
layout(location = 3) out vec2 fUv;
layout(location = 4) out vec4 vPos;
layout (location = 5) flat out uint drawID;

struct ShaderAtlasTexture {
    ivec4 level_index_filtering_assigned;
    vec4 atlasScale_texSize;
};

struct ShaderDrawData {
    mat4 model;
    mat4 mvp;

    ivec4 shadeModel_objectID;
    vec4 albedoColor;
    vec4 metallic_roughness_ambientOcclusion_shininess;

    vec4 diffuseColor;
    vec4 ambientColor;
    vec4 specularColor;

    ShaderAtlasTexture normal;

    ShaderAtlasTexture albedo;
    ShaderAtlasTexture metallic;
    ShaderAtlasTexture roughness;
    ShaderAtlasTexture ambientOcclusion;

    ShaderAtlasTexture diffuse;
    ShaderAtlasTexture ambient;
    ShaderAtlasTexture specular;
    ShaderAtlasTexture shininess;
};

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    ShaderDrawData data[MAX_MULTI_DRAW];
} globs;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

void main()
{
    ShaderDrawData data = globs.data[gl_DrawID];

    vPos = data.mvp * vec4(vPosition, 1);
    fPos = (data.model * vec4(vPosition, 1)).xyz;
    fUv = vUv;

    fNorm = normalize(vNormal);
    fTan = normalize(vTangent);

    gl_Position = vPos;

    drawID = gl_DrawID;
}
)###";

static const char *SHADER_FRAG_GEOMETRY = R"###(#version 460

#include "texfilter.glsl"

#define MAX_MULTI_DRAW 1000

layout(location = 0) in vec3 fPos;
layout(location = 1) in vec3 fNorm;
layout(location = 2) in vec3 fTan;
layout(location = 3) in vec2 fUv;
layout(location = 4) in vec4 vPos;
layout (location = 5) flat in uint drawID;

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oTangent;
layout(location = 3) out vec4 oRoughnessMetallicAO;
layout(location = 4) out vec4 oAlbedo;
layout(location = 5) out vec4 oAmbient;
layout(location = 6) out vec4 oSpecular;
layout(location = 7) out vec4 oModelObject;

struct ShaderAtlasTexture {
    ivec4 level_index_filtering_assigned;
    vec4 atlasScale_texSize;
};

struct ShaderDrawData {
    mat4 model;
    mat4 mvp;

    ivec4 shadeModel_objectID;
    vec4 albedoColor;
    vec4 metallic_roughness_ambientOcclusion_shininess;

    vec4 diffuseColor;
    vec4 ambientColor;
    vec4 specularColor;

    ShaderAtlasTexture normal;

    ShaderAtlasTexture albedo;
    ShaderAtlasTexture metallic;
    ShaderAtlasTexture roughness;
    ShaderAtlasTexture ambientOcclusion;

    ShaderAtlasTexture diffuse;
    ShaderAtlasTexture ambient;
    ShaderAtlasTexture specular;
    ShaderAtlasTexture shininess;
};

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    ShaderDrawData data[MAX_MULTI_DRAW];
} globs;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

vec4 textureAtlas(ShaderAtlasTexture tex, vec2 inUv)
{
    if (tex.level_index_filtering_assigned.w == 0)
    {
        return vec4(0, 0, 0, 0);
    } else {
        vec2 uv = inUv * tex.atlasScale_texSize.xy;
        if (tex.level_index_filtering_assigned.z == 1)
        {
            return textureBicubic(atlasTextures[tex.level_index_filtering_assigned.x],
                                    vec3(uv.x, uv.y, tex.level_index_filtering_assigned.y),
                                    tex.atlasScale_texSize.zw);
        }
        else
        {
            return texture(atlasTextures[tex.level_index_filtering_assigned.x], vec3(uv.x, uv.y, tex.level_index_filtering_assigned.y));
        }
    }
}

void main() {
    ShaderDrawData data = globs.data[drawID];

    oPosition = vec4(fPos, 1);

    if (data.shadeModel_objectID.x == 1)
    {
        oAlbedo = textureAtlas(data.albedo, fUv) + data.albedoColor;
        oRoughnessMetallicAO.r = textureAtlas(data.roughness, fUv).r + data.metallic_roughness_ambientOcclusion_shininess.y;
        oRoughnessMetallicAO.g = textureAtlas(data.metallic, fUv).r + data.metallic_roughness_ambientOcclusion_shininess.x;
        oRoughnessMetallicAO.b = textureAtlas(data.ambientOcclusion, fUv).r + data.metallic_roughness_ambientOcclusion_shininess.z;
    }
    else
    {
        oAlbedo = textureAtlas(data.diffuse, fUv) + data.diffuseColor;
        oAmbient = textureAtlas(data.ambient, fUv) + data.ambientColor;
        oSpecular = textureAtlas(data.specular, fUv) + data.specularColor;
        oRoughnessMetallicAO.r = textureAtlas(data.shininess, fUv).r + data.metallic_roughness_ambientOcclusion_shininess.w;
    }

    mat3 normalMatrix = transpose(inverse(mat3(data.model)));
    oNormal = vec4(normalize(normalMatrix * fNorm), 1);
    oTangent = vec4(normalize(normalMatrix * fTan), 1);

    if (data.normal.level_index_filtering_assigned.w != 0)
    {
        vec3 texNormal = textureAtlas(data.normal, fUv).xyz;
        texNormal = normalize(texNormal * 2.0 - 1.0);
        oNormal = vec4(texNormal, 1);
    }

    oModelObject.r = data.shadeModel_objectID.x;
    oModelObject.g = data.shadeModel_objectID.y;

// Override gbuffer writes with known values for debugging
    oPosition = vec4(1, 1, 1, 1);
    oNormal = vec4(1, 1, 1, 1);
    oTangent = vec4(1, 1, 1, 1);
    oRoughnessMetallicAO = vec4(1, 1, 1, 1);
    oAlbedo = vec4(1, 1, 1, 1);
    oAmbient = vec4(1, 1, 1, 1);
    oSpecular = vec4(1, 1, 1, 1);
    oModelObject = vec4(1, 1, 1, 1);
}
)###";

static const xng::Shader shader = xng::Shader(xng::ShaderSource(SHADER_VERT_GEOMETRY,
                                                                "main",
                                                                xng::VERTEX,
                                                                xng::GLSL_460,
                                                                false),
                                              xng::ShaderSource(SHADER_FRAG_GEOMETRY,
                                                                "main",
                                                                xng::FRAGMENT,
                                                                xng::GLSL_460,
                                                                false),
                                              xng::ShaderSource());

namespace xng {
    static const size_t MAX_MULTI_DRAW = 1000;

    struct ShaderAtlasTexture {
        int level_index_filtering_assigned[4];
        float atlasScale_texSize[4];
    };

    struct ShaderDrawData {
        Mat4f model;
        Mat4f mvp;

        int shadeModel_objectID[4];
        float albedoColor[4];
        float metallic_roughness_ambientOcclusion_shininess[4];

        float diffuseColor[4];
        float ambientColor[4];
        float specularColor[4];

        ShaderAtlasTexture normal;

        ShaderAtlasTexture albedo;
        ShaderAtlasTexture metallic;
        ShaderAtlasTexture roughness;
        ShaderAtlasTexture ambientOcclusion;

        ShaderAtlasTexture diffuse;
        ShaderAtlasTexture ambient;
        ShaderAtlasTexture specular;
        ShaderAtlasTexture shininess;
    };

    struct ShaderBufferData {
        ShaderDrawData data[MAX_MULTI_DRAW];
    };

    GBufferPass::GBufferPass() {}

    void GBufferPass::setup(FrameGraphBuilder &builder) {
        requestedVertexBufferSize = currentVertexBufferSize;
        requestedIndexBufferSize = currentIndexBufferSize;

        renderSize = builder.getBackBufferDescription().size
                     * builder.getProperties().get<float>(FrameGraphProperties::RENDER_SCALE, 1);
        renderTargetRes = builder.createRenderTarget(RenderTargetDesc{
                .size = renderSize,
                .multisample = false,
                .samples = 0,
                .numberOfColorAttachments = 8,
                .hasDepthStencilAttachment = true,
        });
        builder.read(renderTargetRes);
        if (!renderPipelineRes.assigned) {
            auto vs = shader.vertexShader.preprocess(builder.getShaderCompiler(),
                                                     ShaderInclude::getShaderIncludeCallback(),
                                                     ShaderInclude::getShaderMacros(GLSL_460));
            auto fs = shader.fragmentShader.preprocess(builder.getShaderCompiler(),
                                                       ShaderInclude::getShaderIncludeCallback(),
                                                       ShaderInclude::getShaderMacros(GLSL_460));
            vsb = vs.compile(builder.getShaderCompiler());
            fsb = fs.compile(builder.getShaderCompiler());
            renderPipelineRes = builder.createPipeline(RenderPipelineDesc{
                    .shaders = {{VERTEX,   vsb.getShader()},
                                {FRAGMENT, fsb.getShader()}},
                    .bindings = {
                            BIND_SHADER_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER,
                            BIND_TEXTURE_ARRAY_BUFFER
                    },
                    .vertexLayout = Mesh::getDefaultVertexLayout(),
                    .clearColorValue = ColorRGBA(0, 0, 0, 0),
                    .clearColor = true,
                    .clearDepth = true,
                    .enableDepthTest = true,
                    .depthTestWrite = true,
                    .depthTestMode = DEPTH_TEST_LESS,
                    .enableFaceCulling = false,
                    .enableBlending = false
            });
        }

        builder.persist(renderPipelineRes);
        builder.read(renderPipelineRes);

        renderPassRes = builder.createRenderPass(RenderPassDesc{
                .numberOfColorAttachments = 8,
                .hasDepthStencilAttachment = true
        });
        builder.read(renderPassRes);

        shaderBufferRes = builder.createShaderBuffer(ShaderBufferDesc{
                .bufferType = RenderBufferType::HOST_VISIBLE,
                .size = sizeof(ShaderBufferData)
        });
        builder.write(shaderBufferRes);

        vertexArrayObjectRes = builder.createVertexArrayObject(VertexArrayObjectDesc{
                .vertexLayout = Mesh::getDefaultVertexLayout()
        });
        builder.read(vertexArrayObjectRes);
        builder.write(vertexArrayObjectRes);

        auto desc = TextureBufferDesc();
        desc.size = renderSize;

        desc.format = RGBA32F;
        gBufferPosition = builder.createTextureBuffer(desc);
        builder.write(gBufferPosition);
        gBufferNormal = builder.createTextureBuffer(desc);
        builder.write(gBufferNormal);
        gBufferTangent = builder.createTextureBuffer(desc);
        builder.write(gBufferTangent);
        gBufferRoughnessMetallicAmbientOcclusion = builder.createTextureBuffer(desc);
        builder.write(gBufferRoughnessMetallicAmbientOcclusion);

        desc.format = RGBA;
        gBufferAlbedo = builder.createTextureBuffer(desc);
        builder.write(gBufferAlbedo);
        gBufferAmbient = builder.createTextureBuffer(desc);
        builder.write(gBufferAmbient);
        gBufferSpecular = builder.createTextureBuffer(desc);
        builder.write(gBufferSpecular);

        desc.format = RGBA32I;
        gBufferModelObject = builder.createTextureBuffer(desc);
        builder.write(gBufferModelObject);

        desc.format = DEPTH_STENCIL;
        gBufferDepth = builder.createTextureBuffer(desc);
        builder.write(gBufferDepth);

        objects.clear();

        usedTextures.clear();
        usedMeshes.clear();
        auto &tmp = builder.getScene().objects;
        for (auto id = 0; id < tmp.size(); id++) {
            auto &object = tmp.at(id);
            if (object.mesh.assigned()) {
                if (object.material.assigned()) {
                    usedMeshes.insert(object.mesh.getUri());

                    auto &mat = object.material.get();
                    if (mat.transparent
                        || mat.shader.assigned())
                        continue;

                    prepareMeshAllocation(object.mesh);

                    usedTextures.insert(mat.normal.getUri());
                    usedTextures.insert(mat.albedoTexture.getUri());
                    usedTextures.insert(mat.metallicTexture.getUri());
                    usedTextures.insert(mat.roughnessTexture.getUri());
                    usedTextures.insert(mat.ambientOcclusionTexture.getUri());
                    usedTextures.insert(mat.diffuseTexture.getUri());
                    usedTextures.insert(mat.ambientTexture.getUri());
                    usedTextures.insert(mat.specularTexture.getUri());
                    usedTextures.insert(mat.shininessTexture.getUri());

                    if (mat.normal.assigned()
                        && textures.find(mat.normal.getUri()) == textures.end()) {
                        textures[mat.normal.getUri()] = atlas.add(mat.normal.get().getImage().get());
                    }
                    if (mat.albedoTexture.assigned()
                        && textures.find(mat.albedoTexture.getUri()) == textures.end()) {
                        textures[mat.albedoTexture.getUri()] = atlas.add(mat.albedoTexture.get().getImage().get());
                    }
                    if (mat.metallicTexture.assigned()
                        && textures.find(mat.metallicTexture.getUri()) == textures.end()) {
                        textures[mat.metallicTexture.getUri()] = atlas.add(mat.metallicTexture.get().getImage().get());
                    }
                    if (mat.roughnessTexture.assigned()
                        && textures.find(mat.roughnessTexture.getUri()) == textures.end()) {
                        textures[mat.roughnessTexture.getUri()] = atlas.add(
                                mat.roughnessTexture.get().getImage().get());
                    }
                    if (mat.ambientOcclusionTexture.assigned()
                        && textures.find(mat.ambientOcclusionTexture.getUri()) == textures.end()) {
                        textures[mat.ambientOcclusionTexture.getUri()] = atlas.add(
                                mat.ambientOcclusionTexture.get().getImage().get());
                    }
                    if (mat.diffuseTexture.assigned()
                        && textures.find(mat.diffuseTexture.getUri()) == textures.end()) {
                        textures[mat.diffuseTexture.getUri()] = atlas.add(mat.diffuseTexture.get().getImage().get());
                    }
                    if (mat.ambientTexture.assigned()
                        && textures.find(mat.ambientTexture.getUri()) == textures.end()) {
                        textures[mat.ambientTexture.getUri()] = atlas.add(mat.ambientTexture.get().getImage().get());
                    }
                    if (mat.specularTexture.assigned()
                        && textures.find(mat.specularTexture.getUri()) == textures.end()) {
                        textures[mat.specularTexture.getUri()] = atlas.add(mat.specularTexture.get().getImage().get());
                    }
                    if (mat.shininessTexture.assigned()
                        && textures.find(mat.shininessTexture.getUri()) == textures.end()) {
                        textures[mat.shininessTexture.getUri()] = atlas.add(
                                mat.shininessTexture.get().getImage().get());
                    }

                    objects.emplace_back(object);
                }
            }
        }

        mergeFreeVertexBufferRanges();
        mergeFreeIndexBufferRanges();

        atlas.setup(builder);

        if (vertexBufferRes.assigned) {
            builder.read(vertexBufferRes);
            builder.write(vertexBufferRes);
            builder.persist(vertexBufferRes);
        }

        if (indexBufferRes.assigned) {
            builder.read(indexBufferRes);
            builder.write(indexBufferRes);
            builder.persist(indexBufferRes);
        }

        if (currentVertexBufferSize < requestedVertexBufferSize) {
            staleVertexBuffer = vertexBufferRes;
            auto d = VertexBufferDesc();
            d.size = requestedVertexBufferSize;
            vertexBufferRes = builder.createVertexBuffer(d);
            currentVertexBufferSize = d.size;
            builder.read(vertexBufferRes);
            builder.write(vertexBufferRes);
            builder.persist(vertexBufferRes);
        }

        if (currentIndexBufferSize < requestedIndexBufferSize) {
            staleIndexBuffer = indexBufferRes;
            auto d = IndexBufferDesc();
            d.size = requestedIndexBufferSize;
            indexBufferRes = builder.createIndexBuffer(d);
            currentIndexBufferSize = d.size;
            builder.read(indexBufferRes);
            builder.write(indexBufferRes);
            builder.persist(indexBufferRes);
        }

        camera = builder.getScene().camera;
        cameraTransform = builder.getScene().cameraTransform;

        auto &blackBoard = builder.getSharedData();
        blackBoard.set(GEOMETRY_BUFFER_POSITION, gBufferPosition);
        blackBoard.set(GEOMETRY_BUFFER_NORMAL, gBufferNormal);
        blackBoard.set(GEOMETRY_BUFFER_TANGENT, gBufferTangent);
        blackBoard.set(GEOMETRY_BUFFER_ROUGHNESS_METALLIC_AO, gBufferRoughnessMetallicAmbientOcclusion);
        blackBoard.set(GEOMETRY_BUFFER_ALBEDO, gBufferAlbedo);
        blackBoard.set(GEOMETRY_BUFFER_AMBIENT, gBufferAmbient);
        blackBoard.set(GEOMETRY_BUFFER_SPECULAR, gBufferSpecular);
        blackBoard.set(GEOMETRY_BUFFER_MODEL_OBJECT, gBufferModelObject);
        blackBoard.set(GEOMETRY_BUFFER_DEPTH, gBufferDepth);
    }

    void GBufferPass::execute(FrameGraphPassResources &resources) {
        auto atlasBuffers = atlas.getAtlasBuffers(resources);

        auto &target = resources.get<RenderTarget>(renderTargetRes);
        auto &pipeline = resources.get<RenderPipeline>(renderPipelineRes);
        auto &pass = resources.get<RenderPass>(renderPassRes);

        auto &shaderBuffer = resources.get<ShaderBuffer>(shaderBufferRes);
        auto &vertexArrayObject = resources.get<VertexArrayObject>(vertexArrayObjectRes);
        auto &vertexBuffer = resources.get<VertexBuffer>(vertexBufferRes);
        auto &indexBuffer = resources.get<IndexBuffer>(indexBufferRes);

        auto &posTex = resources.get<TextureBuffer>(gBufferPosition);
        auto &normalTex = resources.get<TextureBuffer>(gBufferNormal);
        auto &tanTex = resources.get<TextureBuffer>(gBufferTangent);
        auto &roughMetallicAOTex = resources.get<TextureBuffer>(gBufferRoughnessMetallicAmbientOcclusion);
        auto &albedoTex = resources.get<TextureBuffer>(gBufferAlbedo);
        auto &ambientTex = resources.get<TextureBuffer>(gBufferAmbient);
        auto &specularTex = resources.get<TextureBuffer>(gBufferSpecular);
        auto &modelObjectTex = resources.get<TextureBuffer>(gBufferModelObject);
        auto &depthTex = resources.get<TextureBuffer>(gBufferDepth);

        bool updateVao = false;
        if (staleVertexBuffer.assigned) {
            auto &staleBuffer = resources.get<VertexBuffer>(staleVertexBuffer);
            vertexBuffer.copy(staleBuffer);
            staleVertexBuffer = {};
            updateVao = true;
        }

        if (staleIndexBuffer.assigned) {
            auto &staleBuffer = resources.get<IndexBuffer>(staleIndexBuffer);
            indexBuffer.copy(staleBuffer);
            staleIndexBuffer = {};
            updateVao = true;
        }

        if (updateVao) {
            vertexArrayObject.bindBuffers(vertexBuffer, indexBuffer);
        }

        allocateMeshes(vertexBuffer, indexBuffer);

        // Deallocate unused meshes
        std::set<Uri> dealloc;
        for (auto &pair: meshAllocations) {
            if (usedMeshes.find(pair.first) == usedMeshes.end()) {
                dealloc.insert(pair.first);
            }
        }
        for (auto &uri: dealloc) {
            deallocateMesh(ResourceHandle<Mesh>(uri));
        }

        // Deallocate unused textures
        dealloc.clear();
        for (auto &pair: textures) {
            if (usedTextures.find(pair.first) == usedTextures.end()) {
                dealloc.insert(pair.first);
            }
        }
        for (auto &uri: dealloc) {
            deallocateTexture(ResourceHandle<Texture>(uri));
        }

        target.setColorAttachments({
                                           posTex,
                                           normalTex,
                                           tanTex,
                                           roughMetallicAOTex,
                                           albedoTex,
                                           ambientTex,
                                           specularTex,
                                           modelObjectTex
                                   });

        target.setDepthStencilAttachment(&depthTex);

        auto projection = camera.projection();
        auto view = Camera::view(cameraTransform);

        auto iterations = objects.size() > MAX_MULTI_DRAW ? objects.size() / MAX_MULTI_DRAW : 1;

        pass.beginRenderPass(target, {}, target.getDescription().size);
        pass.bindPipeline(pipeline);
        pass.bindVertexArrayObject(vertexArrayObject);

        for (auto i = 0; i < iterations; i++) {
            std::vector<RenderPass::DrawCall> drawCalls;
            std::vector<size_t> baseVertices;
            ShaderBufferData shaderData;
            for (auto oi = i * MAX_MULTI_DRAW; oi < MAX_MULTI_DRAW && oi < objects.size(); oi++) {
                auto &object = objects.at(oi);
                auto model = object.transform.model();
                auto &material = object.material.get();

                auto data = ShaderDrawData();

                data.model = model;
                data.mvp = projection * view * model;
                data.shadeModel_objectID[0] = material.shadingModel;
                data.shadeModel_objectID[1] = static_cast<int>(oi);

                auto col = material.albedo.divide().getMemory();
                data.albedoColor[0] = col[0];
                data.albedoColor[1] = col[1];
                data.albedoColor[2] = col[2];
                data.albedoColor[3] = col[3];

                data.metallic_roughness_ambientOcclusion_shininess[0] = material.metallic;
                data.metallic_roughness_ambientOcclusion_shininess[1] = material.roughness;
                data.metallic_roughness_ambientOcclusion_shininess[2] = material.ambientOcclusion;
                data.metallic_roughness_ambientOcclusion_shininess[3] = material.shininess;

                col = material.diffuse.divide().getMemory();
                data.diffuseColor[0] = col[0];
                data.diffuseColor[1] = col[1];
                data.diffuseColor[2] = col[2];
                data.diffuseColor[3] = col[3];

                col = material.ambient.divide().getMemory();
                data.ambientColor[0] = col[0];
                data.ambientColor[1] = col[1];
                data.ambientColor[2] = col[2];
                data.ambientColor[3] = col[3];

                col = material.specular.divide().getMemory();
                data.specularColor[0] = col[0];
                data.specularColor[1] = col[1];
                data.specularColor[2] = col[2];
                data.specularColor[3] = col[3];

                if (material.albedoTexture.assigned()) {
                    auto tex = getTexture(material.albedoTexture, atlasBuffers);

                    data.albedo.level_index_filtering_assigned[0] = tex.level;
                    data.albedo.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.albedo.level_index_filtering_assigned[2] = material.albedoTexture.get().description.filterMag;
                    data.albedo.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.albedo.atlasScale_texSize[0] = atlasScale.x;
                    data.albedo.atlasScale_texSize[1] = atlasScale.y;
                    data.albedo.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.albedo.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                } else {
                    data.albedo.level_index_filtering_assigned[3] = 0;
                }

                if (material.metallicTexture.assigned()) {
                    auto tex = getTexture(material.metallicTexture, atlasBuffers);

                    data.metallic.level_index_filtering_assigned[0] = tex.level;
                    data.metallic.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.metallic.level_index_filtering_assigned[2] = material.metallicTexture.get().description.filterMag;
                    data.metallic.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.metallic.atlasScale_texSize[0] = atlasScale.x;
                    data.metallic.atlasScale_texSize[1] = atlasScale.y;
                    data.metallic.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.metallic.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                } else {
                    data.metallic.level_index_filtering_assigned[3] = 0;
                }

                if (material.roughnessTexture.assigned()) {
                    auto tex = getTexture(material.roughnessTexture, atlasBuffers);

                    data.roughness.level_index_filtering_assigned[0] = tex.level;
                    data.roughness.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.roughness.level_index_filtering_assigned[2] = material.roughnessTexture.get().description.filterMag;
                    data.roughness.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.roughness.atlasScale_texSize[0] = atlasScale.x;
                    data.roughness.atlasScale_texSize[1] = atlasScale.y;
                    data.roughness.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.roughness.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                } else {
                    data.roughness.level_index_filtering_assigned[3] = 0;
                }

                if (material.ambientOcclusionTexture.assigned()) {
                    auto tex = getTexture(material.ambientOcclusionTexture, atlasBuffers);

                    data.ambientOcclusion.level_index_filtering_assigned[0] = tex.level;
                    data.ambientOcclusion.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.ambientOcclusion.level_index_filtering_assigned[2] = material.ambientOcclusionTexture.get().description.filterMag;
                    data.ambientOcclusion.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.ambientOcclusion.atlasScale_texSize[0] = atlasScale.x;
                    data.ambientOcclusion.atlasScale_texSize[1] = atlasScale.y;
                    data.ambientOcclusion.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.ambientOcclusion.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                } else {
                    data.ambientOcclusion.level_index_filtering_assigned[3] = 0;
                }

                if (material.diffuseTexture.assigned()) {
                    auto tex = getTexture(material.diffuseTexture, atlasBuffers);

                    data.diffuse.level_index_filtering_assigned[0] = tex.level;
                    data.diffuse.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.diffuse.level_index_filtering_assigned[2] = material.diffuseTexture.get().description.filterMag;
                    data.diffuse.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.diffuse.atlasScale_texSize[0] = atlasScale.x;
                    data.diffuse.atlasScale_texSize[1] = atlasScale.y;
                    data.diffuse.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.diffuse.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                } else {
                    data.diffuse.level_index_filtering_assigned[3] = 0;
                }

                if (material.ambientTexture.assigned()) {
                    auto tex = getTexture(material.ambientTexture, atlasBuffers);

                    data.ambient.level_index_filtering_assigned[0] = tex.level;
                    data.ambient.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.ambient.level_index_filtering_assigned[2] = material.ambientTexture.get().description.filterMag;
                    data.ambient.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.ambient.atlasScale_texSize[0] = atlasScale.x;
                    data.ambient.atlasScale_texSize[1] = atlasScale.y;
                    data.ambient.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.ambient.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                } else {
                    data.ambient.level_index_filtering_assigned[3] = 0;
                }

                if (material.specularTexture.assigned()) {
                    auto tex = getTexture(material.specularTexture, atlasBuffers);

                    data.specular.level_index_filtering_assigned[0] = tex.level;
                    data.specular.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.specular.level_index_filtering_assigned[2] = material.specularTexture.get().description.filterMag;
                    data.specular.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.specular.atlasScale_texSize[0] = atlasScale.x;
                    data.specular.atlasScale_texSize[1] = atlasScale.y;
                    data.specular.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.specular.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                } else {
                    data.specular.level_index_filtering_assigned[3] = 0;
                }

                if (material.shininessTexture.assigned()) {
                    auto tex = getTexture(material.shininessTexture, atlasBuffers);

                    data.shininess.level_index_filtering_assigned[0] = tex.level;
                    data.shininess.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.shininess.level_index_filtering_assigned[2] = material.shininessTexture.get().description.filterMag;
                    data.shininess.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.shininess.atlasScale_texSize[0] = atlasScale.x;
                    data.shininess.atlasScale_texSize[1] = atlasScale.y;
                    data.shininess.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.shininess.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                } else {
                    data.albedo.level_index_filtering_assigned[3] = 0;
                }

                if (material.normal.assigned()) {
                    auto tex = getTexture(material.normal, atlasBuffers);

                    data.normal.level_index_filtering_assigned[0] = tex.level;
                    data.normal.level_index_filtering_assigned[1] = static_cast<int>(tex.index);
                    data.normal.level_index_filtering_assigned[2] = material.normal.get().description.filterMag;
                    data.normal.level_index_filtering_assigned[3] = 1;

                    auto atlasScale = tex.size.convert<float>()
                                      / TextureAtlas::getResolutionLevelSize(tex.level).convert<float>();

                    data.normal.atlasScale_texSize[0] = atlasScale.x;
                    data.normal.atlasScale_texSize[1] = atlasScale.y;
                    data.normal.atlasScale_texSize[2] = static_cast<float>(tex.size.x);
                    data.normal.atlasScale_texSize[3] = static_cast<float>(tex.size.y);
                } else {
                    data.albedo.level_index_filtering_assigned[3] = 0;
                }

                shaderData.data[oi - (i * MAX_MULTI_DRAW)] = data;

                auto mesh = getMesh(object.mesh);

                drawCalls.emplace_back(mesh.drawCall);
                baseVertices.emplace_back(mesh.baseVertex);
            }
            shaderBuffer.upload(shaderData);
            pass.bindShaderData({shaderBuffer,
                                 atlasBuffers.at(TEXTURE_ATLAS_8x8),
                                 atlasBuffers.at(TEXTURE_ATLAS_16x16),
                                 atlasBuffers.at(TEXTURE_ATLAS_32x32),
                                 atlasBuffers.at(TEXTURE_ATLAS_64x64),
                                 atlasBuffers.at(TEXTURE_ATLAS_128x128),
                                 atlasBuffers.at(TEXTURE_ATLAS_256x256),
                                 atlasBuffers.at(TEXTURE_ATLAS_512x512),
                                 atlasBuffers.at(TEXTURE_ATLAS_1024x1024),
                                 atlasBuffers.at(TEXTURE_ATLAS_2048x2048),
                                 atlasBuffers.at(TEXTURE_ATLAS_4096x4096),
                                 atlasBuffers.at(TEXTURE_ATLAS_8192x8192),
                                 atlasBuffers.at(TEXTURE_ATLAS_16384x16384)});
            pass.multiDrawIndexed(drawCalls, baseVertices);
        }

        pass.endRenderPass();

        target.setColorAttachments({});
        target.setDepthStencilAttachment(nullptr);
    }

    std::type_index GBufferPass::getTypeIndex() const {
        return typeid(GBufferPass);
    }

    TextureAtlasHandle GBufferPass::getTexture(const ResourceHandle<Texture> &texture,
                                               std::map<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>> &atlasBuffers) {
        if (textures.find(texture.getUri()) == textures.end()) {
            auto handle = atlas.add(texture.get().getImage().get());
            textures[texture.getUri()] = handle;
        }
        return textures.at(texture.getUri());
    }

    GBufferPass::MeshDrawData GBufferPass::getMesh(const ResourceHandle<Mesh> &mesh) {
        return meshAllocations.at(mesh.getUri());
    }

    void GBufferPass::prepareMeshAllocation(const ResourceHandle<Mesh> &mesh) {
        if (mesh.get().primitive != TRIANGLES) {
            throw std::runtime_error("Unsupported mesh primitive");
        } else if (mesh.get().vertexLayout != Mesh::getDefaultVertexLayout()) {
            throw std::runtime_error("Unsupported mesh vertex layout");
        } else if (mesh.get().indices.empty()) {
            throw std::runtime_error("Arrayed mesh not supported, must be indexed");
        }
        if (meshAllocations.find(mesh.getUri()) == meshAllocations.end()
            && pendingMeshAllocations.find(mesh.getUri()) == pendingMeshAllocations.end()) {
            MeshDrawData data;
            data.primitive = mesh.get().primitive;
            data.drawCall.count = mesh.get().indices.size();
            data.drawCall.offset = allocateIndexData(mesh.get().indices.size() * sizeof(unsigned int));
            data.baseVertex = allocateVertexData(mesh.get().vertices.size() * mesh.get().vertexLayout.getSize())
                    / mesh.get().vertexLayout.getSize();
            pendingMeshAllocations[mesh.getUri()] = data;
            pendingMeshHandles[mesh.getUri()] = mesh;
        }
    }

    void GBufferPass::allocateMeshes(VertexBuffer &vertexBuffer, IndexBuffer &indexBuffer) {
        for (auto &pair: pendingMeshAllocations) {
            auto mesh = pendingMeshHandles.at(pair.first);
            auto buf = VertexStream().addVertices(mesh.get().vertices).getVertexBuffer();
            indexBuffer.upload(pair.second.drawCall.offset,
                               reinterpret_cast<const uint8_t *>(mesh.get().indices.data()),
                               mesh.get().indices.size() * sizeof(unsigned int));
            vertexBuffer.upload(pair.second.baseVertex * mesh.get().vertexLayout.getSize(),
                                buf.data(),
                                buf.size());
            meshAllocations[pair.first] = pair.second;
        }
        pendingMeshAllocations.clear();
        pendingMeshHandles.clear();
    }

    void GBufferPass::deallocateMesh(const ResourceHandle<Mesh> &mesh) {
        auto alloc = meshAllocations.at(mesh.getUri());
        meshAllocations.erase(mesh.getUri());
        deallocateVertexData(alloc.baseVertex * mesh.get().vertexLayout.getSize());
        deallocateIndexData(alloc.drawCall.offset);
    }

    void GBufferPass::deallocateTexture(const ResourceHandle<Texture> &texture) {
        atlas.remove(textures.at(texture.getUri()));
        textures.erase(texture.getUri());
    }

    size_t GBufferPass::allocateVertexData(size_t size) {
        assert(size % Mesh::getDefaultVertexLayout().getSize() == 0);

        bool foundFreeRange = false;
        auto ret = 0UL;
        for (auto &range: freeVertexBufferRanges) {
            if (range.second >= size) {
                ret = range.first;
                foundFreeRange = true;
            }
        }

        if (foundFreeRange) {
            auto rangeSize = freeVertexBufferRanges.at(ret) -= size;
            freeVertexBufferRanges.erase(ret);
            if (rangeSize > 0) {
                freeVertexBufferRanges[ret + size] = rangeSize;
            }
        } else {
            ret = requestedVertexBufferSize;
            requestedVertexBufferSize += size;
        }

        allocatedVertexRanges[ret] = size;

        return ret;
    }

    void GBufferPass::deallocateVertexData(size_t offset) {
        auto size = allocatedVertexRanges.at(offset);
        allocatedVertexRanges.erase(offset);
        freeVertexBufferRanges[offset] = size;
    }

    size_t GBufferPass::allocateIndexData(size_t size) {
        bool foundFreeRange = false;
        auto ret = 0UL;
        for (auto &range: freeIndexBufferRanges) {
            if (range.second >= size) {
                ret = range.first;
                foundFreeRange = true;
            }
        }
        if (foundFreeRange) {
            auto rangeSize = freeIndexBufferRanges.at(ret) -= size;
            freeIndexBufferRanges.erase(ret);
            if (rangeSize > 0) {
                freeIndexBufferRanges[ret + size] = rangeSize;
            }
        } else {
            ret = requestedIndexBufferSize;
            requestedIndexBufferSize += size;
        }

        allocatedIndexRanges[ret] = size;

        return ret;
    }

    void GBufferPass::deallocateIndexData(size_t offset) {
        auto size = allocatedIndexRanges.at(offset);
        allocatedIndexRanges.erase(offset);
        freeIndexBufferRanges[offset] = size;
    }

    void GBufferPass::mergeFreeVertexBufferRanges() {
        bool merged = true;
        while (merged) {
            merged = false;
            auto vertexRanges = freeVertexBufferRanges;
            for (auto range = freeVertexBufferRanges.begin(); range != freeVertexBufferRanges.end(); range++) {
                auto next = range;
                next++;
                if (next != freeVertexBufferRanges.end()) {
                    if (range->first + range->second == next->first
                        && vertexRanges.find(range->first) != vertexRanges.end()
                        && vertexRanges.find(next->first) != vertexRanges.end()) {
                        merged = true;
                        vertexRanges.at(range->first) += next->second;
                        vertexRanges.erase(next->first);
                        range = next;
                    }
                }
            }
            freeVertexBufferRanges = vertexRanges;
        }
    }

    void GBufferPass::mergeFreeIndexBufferRanges() {
        bool merged = true;
        while (merged) {
            merged = false;
            auto indexRanges = freeIndexBufferRanges;
            for (auto range = freeIndexBufferRanges.begin(); range != freeIndexBufferRanges.end(); range++) {
                auto next = range;
                next++;
                if (next != freeIndexBufferRanges.end()) {
                    if (range->first + range->second == next->first
                        && indexRanges.find(range->first) != indexRanges.end()
                        && indexRanges.find(next->first) != indexRanges.end()) {
                        merged = true;
                        indexRanges.at(range->first) += next->second;
                        indexRanges.erase(next->first);
                        range = next;
                    }
                }
            }
            freeIndexBufferRanges = indexRanges;
        }
    }
}
