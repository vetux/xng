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

#include "xng/graphics/passes/iblprepass.hpp"

#include "../../../include/xng/rendergraph/image.hpp"
#include "xng/graphics/scene/mesh.hpp"
#include "xng/rendergraph/shaderscript/shaderscript.hpp"
#include "xng/rendergraph/rendergraphattachment.hpp"
#include "xng/math/matrixmath.hpp"
#include "xng/math/vector3.hpp"

namespace xng
{
    IBLPrePass::IBLPrePass(std::shared_ptr<RenderConfiguration> config,
                           std::shared_ptr<SharedResourceRegistry> registry)
        : config(std::move(config)), registry(std::move(registry))
    {
    }

    void IBLPrePass::create(RenderGraphBuilder& builder)
    {
        cubemapSize = config->iblCubemapSize;
        prefilterSize = config->iblPrefilterSize;
        irradianceSize = config->iblIrradianceSize;
        brdfSize = config->iblBRDFSize;

        // create irradiance cubemap (small)
        RenderGraphTextureBuffer irrDesc;
        irrDesc.size = irradianceSize;
        irrDesc.textureType = TEXTURE_CUBE_MAP;
        irrDesc.format = RGBA16F;
        irrDesc.mipMapLevels = 1;
        irrDesc.filterMin = LINEAR;
        irrDesc.filterMag = LINEAR;
        irrDesc.wrapping = CLAMP_TO_EDGE;
        irradiance = builder.createTexture(irrDesc);

        // create prefilter cubemap (mipmapped)
        RenderGraphTextureBuffer preDesc;
        preDesc.size = prefilterSize;
        preDesc.textureType = TEXTURE_CUBE_MAP;
        preDesc.format = RGBA16F;
        preDesc.mipMapLevels = RenderGraphTextureBuffer::calculateMipLevels(preDesc.size);
        preDesc.filterMin = LINEAR;
        preDesc.filterMag = LINEAR;
        preDesc.mipMapFilter = LINEAR_MIPMAP_LINEAR;
        preDesc.wrapping = CLAMP_TO_EDGE;
        prefilter = builder.createTexture(preDesc);

        // create BRDF LUT 2D
        RenderGraphTextureBuffer lutDesc;
        lutDesc.size = brdfSize;
        lutDesc.textureType = TEXTURE_2D;
        lutDesc.format = RG16F;
        lutDesc.mipMapLevels = 1;
        lutDesc.filterMin = LINEAR;
        lutDesc.filterMag = LINEAR;
        lutDesc.wrapping = CLAMP_TO_EDGE;
        brdfLut = builder.createTexture(lutDesc);

        // create a fullscreen quad vertex buffer used for BRDF LUT
        auto quad = Mesh::normalizedQuad();
        vertexBuffer = builder.createVertexBuffer(quad.vertices.size());

        // create cube mesh buffers used for cubemap rendering - use subdivided cube for better interpolation
        auto cube = Mesh::normalizedCube(32);
        cubeVertexBuffer = builder.createVertexBuffer(cube.vertices.size());
        indexBuffer = builder.createIndexBuffer(cube.indices.size() * sizeof(unsigned int));

        // create a small shader buffer to upload a single capture matrix per draw
        captureBuffer = builder.createShaderBuffer(sizeof(Mat4f));

        // create a small shader buffer for prefilter params (roughness, samples)
        prefilterParamBuffer = builder.createShaderBuffer(sizeof(float) * 4);

        pipelineBRDF = builder.createPipeline(makeBRDFPipeline());
        pipelineEquirectToCube = builder.createPipeline(makeEquirectToCubePipeline());
        pipelineIrradiance = builder.createPipeline(makeIrradiancePipeline());
        pipelinePrefilter = builder.createPipeline(makePrefilterPipeline());

        // create an intermediate environment cubemap that will be populated from the HDR equirect
        RenderGraphTextureBuffer envCubeDesc;
        envCubeDesc.size = cubemapSize;
        envCubeDesc.textureType = TEXTURE_CUBE_MAP;
        envCubeDesc.format = RGBA16F;
        envCubeDesc.mipMapLevels = RenderGraphTextureBuffer::calculateMipLevels(envCubeDesc.size);
        envCubeDesc.filterMin = LINEAR;
        envCubeDesc.filterMag = LINEAR;
        envCubeDesc.mipMapFilter = LINEAR_MIPMAP_LINEAR;
        envCubeDesc.wrapping = CLAMP_TO_EDGE;
        environmentCube = builder.createTexture(envCubeDesc);

        const auto& scene = config->getScene();

        currentHDRI = scene.hdri;

        if (currentHDRI.assigned())
        {
            const auto& img = currentHDRI.get();
            RenderGraphTextureBuffer desc;
            desc.size = img.getResolution();
            desc.textureType = TEXTURE_2D;
            desc.format = RGB32F;
            desc.mipMapLevels = 1;
            desc.filterMin = LINEAR;
            desc.filterMag = LINEAR;
            desc.wrapping = CLAMP_TO_EDGE;
            env = builder.createTexture(desc);
        }

        // register IBL maps in shared registry (environment left for loader to set if it already has a cubemap)
        IBLMaps ibl;
        ibl.environment = environmentCube;
        ibl.irradiance = irradiance;
        ibl.prefilter = prefilter;
        ibl.brdfLUT = brdfLut;
        registry->set<IBLMaps>(ibl);

        // Add pass and declare resource usage
        auto pass = builder.addPass("IBLPrePass", [this](RenderGraphContext& ctx)
        {
            runPass(ctx);
        });

        builder.read(pass, pipelineBRDF);
        builder.read(pass, pipelineEquirectToCube);
        builder.read(pass, pipelineIrradiance);
        builder.read(pass, pipelinePrefilter);
        builder.readWrite(pass, vertexBuffer);
        builder.readWrite(pass, cubeVertexBuffer);
        builder.readWrite(pass, indexBuffer);
        builder.readWrite(pass, captureBuffer);
        builder.readWrite(pass, prefilterParamBuffer);
        builder.write(pass, environmentCube);
        builder.write(pass, irradiance);
        builder.write(pass, prefilter);
        builder.write(pass, brdfLut);
    }

    void IBLPrePass::recreate(RenderGraphBuilder& builder)
    {
        if (cubemapSize == config->iblCubemapSize)
        {
            environmentCube = builder.inheritResource(environmentCube);
        }
        else
        {
            RenderGraphTextureBuffer envCubeDesc;
            envCubeDesc.size = cubemapSize;
            envCubeDesc.textureType = TEXTURE_CUBE_MAP;
            envCubeDesc.format = RGBA16F;
            envCubeDesc.mipMapLevels = RenderGraphTextureBuffer::calculateMipLevels(envCubeDesc.size);
            envCubeDesc.filterMin = LINEAR;
            envCubeDesc.filterMag = LINEAR;
            envCubeDesc.mipMapFilter = LINEAR_MIPMAP_LINEAR;
            envCubeDesc.wrapping = CLAMP_TO_EDGE;
            environmentCube = builder.createTexture(envCubeDesc);
        }

        if (irradianceSize == config->iblIrradianceSize)
        {
            irradiance = builder.inheritResource(irradiance);
        }
        else
        {
            // create irradiance cubemap (small)
            RenderGraphTextureBuffer irrDesc;
            irrDesc.size = irradianceSize;
            irrDesc.textureType = TEXTURE_CUBE_MAP;
            irrDesc.format = RGBA16F;
            irrDesc.mipMapLevels = 1;
            irrDesc.filterMin = LINEAR;
            irrDesc.filterMag = LINEAR;
            irrDesc.wrapping = CLAMP_TO_EDGE;
            irradiance = builder.createTexture(irrDesc);
        }

        if (prefilterSize == config->iblPrefilterSize)
        {
            prefilter = builder.inheritResource(prefilter);
        }
        else
        {
            // create prefilter cubemap (mipmapped)
            RenderGraphTextureBuffer preDesc;
            preDesc.size = prefilterSize;
            preDesc.textureType = TEXTURE_CUBE_MAP;
            preDesc.format = RGBA16F;
            preDesc.mipMapLevels = RenderGraphTextureBuffer::calculateMipLevels(preDesc.size);
            preDesc.filterMin = LINEAR;
            preDesc.filterMag = LINEAR;
            preDesc.mipMapFilter = LINEAR_MIPMAP_LINEAR;
            preDesc.wrapping = CLAMP_TO_EDGE;
            prefilter = builder.createTexture(preDesc);
        }

        if (brdfSize == config->iblBRDFSize)
        {
            brdfLut = builder.inheritResource(brdfLut);
        }
        else
        {
            // create BRDF LUT 2D
            RenderGraphTextureBuffer lutDesc;
            lutDesc.size = brdfSize;
            lutDesc.textureType = TEXTURE_2D;
            lutDesc.format = RG16F;
            lutDesc.mipMapLevels = 1;
            lutDesc.filterMin = LINEAR;
            lutDesc.filterMag = LINEAR;
            lutDesc.wrapping = CLAMP_TO_EDGE;
            brdfLut = builder.createTexture(lutDesc);
        }

        cubeVertexBuffer = builder.inheritResource(cubeVertexBuffer);
        indexBuffer = builder.inheritResource(indexBuffer);
        vertexBuffer = builder.inheritResource(vertexBuffer);
        captureBuffer = builder.inheritResource(captureBuffer);
        prefilterParamBuffer = builder.inheritResource(prefilterParamBuffer);
        pipelineBRDF = builder.inheritResource(pipelineBRDF);
        pipelineEquirectToCube = builder.inheritResource(pipelineEquirectToCube);
        pipelineIrradiance = builder.inheritResource(pipelineIrradiance);
        pipelinePrefilter = builder.inheritResource(pipelinePrefilter);

        IBLMaps ibl = registry->getOrCreate<IBLMaps>();
        ibl.environment = environmentCube;
        ibl.irradiance = irradiance;
        ibl.prefilter = prefilter;
        ibl.brdfLUT = brdfLut;
        registry->set<IBLMaps>(ibl);

        const auto& scene = config->getScene();

        currentHDRI = scene.hdri;

        if (currentHDRI.getUri() == builtHDRI)
        {
            env = builder.inheritResource(env);
        }
        else
        {
            const auto& img = currentHDRI.get();
            RenderGraphTextureBuffer desc;
            desc.size = img.getResolution();
            desc.textureType = TEXTURE_2D;
            desc.format = RGB32F;
            desc.mipMapLevels = 1;
            env = builder.createTexture(desc);
        }

        builder.addPass("IBLPrePass", [this](RenderGraphContext& ctx)
        {
            runPass(ctx);
        });
    }

    bool IBLPrePass::shouldRebuild(const Vec2i& backBufferSize)
    {
        return currentHDRI.getUri() != builtHDRI
            || cubemapSize != config->iblCubemapSize
            || prefilterSize != config->iblPrefilterSize
            || irradianceSize != config->iblIrradianceSize
            || brdfSize != config->iblBRDFSize;
    }

    void IBLPrePass::runPass(RenderGraphContext& c)
    {
        if (!currentHDRI.assigned() ||
            (currentHDRI.getUri() == builtHDRI
                && cubemapSize == config->iblCubemapSize
                && prefilterSize == config->iblPrefilterSize
                && irradianceSize == config->iblIrradianceSize
                && brdfSize == config->iblBRDFSize))
        {
            return;
        }

        cubemapSize = config->iblCubemapSize;
        prefilterSize = config->iblPrefilterSize;
        irradianceSize = config->iblIrradianceSize;
        brdfSize = config->iblBRDFSize;

        builtHDRI = currentHDRI.getUri();

        PassBuilder builder(ctx);

        builder.addPass([](TransferContext& ctx) {
            // Upload quad vertices once
            const auto& quad = Mesh::normalizedQuad();
            static bool quadUploaded = false;
            if (!quadUploaded)
            {
                quadUploaded = true;
                ctx.uploadBuffer(vertexBuffer, quad.vertices.data(), quad.vertices.size(), 0);
            }

            // Upload cube mesh vertices/indices once (using subdivided cube)
            const auto cube = Mesh::normalizedCube(32);
            static bool cubeUploaded = false;
            if (!cubeUploaded)
            {
                cubeUploaded = true;
                ctx.uploadBuffer(cubeVertexBuffer, cube.vertices.data(), cube.vertices.size(), 0);
                ctx.uploadBuffer(indexBuffer, reinterpret_cast<const uint8_t*>(cube.indices.data()),
                                 cube.indices.size() * sizeof(unsigned int), 0);
            }

            // Upload the HDR equirectangular texture and convert it to cubemap
            std::array<Mat4f, 6> views;
            const auto& img = currentHDRI.get();
            const auto& buf = img.getBuffer();
            if (!buf.empty())
            {
                ctx.uploadTexture(env,
                                  reinterpret_cast<const uint8_t*>(buf.data()),
                                  buf.size() * sizeof(ColorRGBF),
                                  RGB32F,
                                  0,
                                  CubeMapFace{},
                                  0,
                                  img.getResolution(),
                                  {0, 0});
            }
        })
        .access(vertexBuffer);

        // proceed with conversion to cubemap
        // Use right-handed capture matrices. MatrixMath::lookAt/perspective use
        // GLM_FORCE_LEFT_HANDED, but OpenGL cubemap lookup always uses a right-handed
        // convention, so we use the explicit RH variants.
        const float near = 0.1f;
        const float far = 10.0f;
        Mat4f proj = MatrixMath::perspectiveRH(90.0f, 1.0f, near, far);
        views[0] = proj * MatrixMath::lookAtRH(Vec3f(0, 0, 0), Vec3f(1, 0, 0), Vec3f(0, -1, 0));
        views[1] = proj * MatrixMath::lookAtRH(Vec3f(0, 0, 0), Vec3f(-1, 0, 0), Vec3f(0, -1, 0));
        views[2] = proj * MatrixMath::lookAtRH(Vec3f(0, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, 1));
        views[3] = proj * MatrixMath::lookAtRH(Vec3f(0, 0, 0), Vec3f(0, -1, 0), Vec3f(0, 0, -1));
        views[4] = proj * MatrixMath::lookAtRH(Vec3f(0, 0, 0), Vec3f(0, 0, 1), Vec3f(0, -1, 0));
        views[5] = proj * MatrixMath::lookAtRH(Vec3f(0, 0, 0), Vec3f(0, 0, -1), Vec3f(0, -1, 0));

        for (int face = 0; face < 6; ++face)
        {
            builder.addPass([](TransferContext &) {
                ctx.uploadBuffer(captureBuffer, reinterpret_cast<const uint8_t*>(&views[face]), sizeof(Mat4f), 0);
            });
            builder.addPass([](RasterContext& ctx) {
                ctx.beginRenderPass({
                                        RenderGraphAttachment(environmentCube,
                                                              0,
                                                              static_cast<CubeMapFace>(face),
                                                              0)
                                    },
                                    {});
                ctx.setViewport({}, cubemapSize);
                // bind the pipeline that samples the equirectangular HDR and writes to cubemap
                ctx.bindPipeline(pipelineEquirectToCube);
                ctx.bindVertexBuffer(cubeVertexBuffer);
                ctx.bindIndexBuffer(indexBuffer);
                ctx.bindShaderBuffer("captureData", captureBuffer);
                ctx.bindTexture("envEquirect", env);
                ctx.drawIndexed(DrawCall(0, cube.indices.size()), 0);
                ctx.endRenderPass();
            })
            .attachColor(environmentCube);
        }

        // Generate mipmaps for the environment cube so it can be sampled with filtering
        ctx.generateMipMaps(environmentCube);

        // Generate irradiance cubemap by convolving the environmentCube
        // Upload environment cubemap resolution for LOD computation in the irradiance shader
        float irrParams[4] = {0.0f, (float)cubemapSize.x, (float)cubemapSize.y, 0.0f};
        ctx.uploadBuffer(prefilterParamBuffer, reinterpret_cast<const uint8_t*>(&irrParams),
                         sizeof(irrParams), 0);

        for (int face = 0; face < 6; ++face)
        {
            // upload per-face view-projection matrix
            ctx.uploadBuffer(captureBuffer, reinterpret_cast<const uint8_t*>(&views[face]), sizeof(Mat4f), 0);
            ctx.beginRenderPass({
                                    RenderGraphAttachment(irradiance,
                                                          0,
                                                          static_cast<CubeMapFace>(face),
                                                          0)
                                },
                                {});
            // match the irradiance texture size configured in create()
            ctx.setViewport({}, irradianceSize);
            ctx.bindPipeline(pipelineIrradiance);
            ctx.bindVertexBuffer(cubeVertexBuffer);
            ctx.bindIndexBuffer(indexBuffer);
            ctx.bindShaderBuffer("captureData", captureBuffer);
            ctx.bindShaderBuffer("prefilterParams", prefilterParamBuffer);
            ctx.bindTexture("envCube", environmentCube);
            ctx.drawIndexed(DrawCall(0, cube.indices.size()), 0);
            ctx.endRenderPass();
        }

        // Generate prefiltered environment map (specular) across mip levels
        int maxMip = RenderGraphTextureBuffer::calculateMipLevels(prefilterSize) - 1;
        for (int mip = 0; mip <= maxMip; ++mip)
        {
            Vec2i mipSize = {std::max(1, prefilterSize.x >> mip), std::max(1, prefilterSize.y >> mip)};
            float roughness = (float)mip / (float)maxMip;
            // upload roughness and base prefilter resolution into prefilter param buffer
            // resolution must be the SOURCE (environment) cubemap size for correct solid-angle computation
            float prefilterParams[4] = {roughness, (float)cubemapSize.x, (float)cubemapSize.y, 0.0f};
            ctx.uploadBuffer(prefilterParamBuffer, reinterpret_cast<const uint8_t*>(&prefilterParams),
                             sizeof(prefilterParams), 0);

            for (int face = 0; face < 6; ++face)
            {
                // upload capture matrix for this face so the vertex shader orients the cube correctly
                ctx.uploadBuffer(captureBuffer, reinterpret_cast<const uint8_t*>(&views[face]), sizeof(Mat4f), 0);

                ctx.beginRenderPass({
                                        RenderGraphAttachment(prefilter,
                                                              0,
                                                              static_cast<CubeMapFace>(face),
                                                              static_cast<size_t>(mip))
                                    },
                                    {});
                ctx.setViewport({}, mipSize);
                ctx.bindPipeline(pipelinePrefilter);
                ctx.bindVertexBuffer(cubeVertexBuffer);
                ctx.bindIndexBuffer(indexBuffer);
                ctx.bindShaderBuffer("captureData", captureBuffer);
                ctx.bindShaderBuffer("prefilterParams", prefilterParamBuffer);
                ctx.bindTexture("envCube", environmentCube);
                ctx.drawIndexed(DrawCall(0, cube.indices.size()), 0);
                ctx.endRenderPass();
            }
        }

        // Generate BRDF integration LUT
        ctx.beginRenderPass({RenderGraphAttachment(brdfLut)}, {});
        ctx.setViewport({}, brdfSize);
        ctx.bindPipeline(pipelineBRDF);
        ctx.bindVertexBuffer(vertexBuffer);
        ctx.drawArray(DrawCall(0, 6));
        ctx.endRenderPass();
    }
}
