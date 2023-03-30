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

#ifndef XENGINE_TESTPASS_HPP
#define XENGINE_TESTPASS_HPP

#include "xng/xng.hpp"

using namespace xng;

static const char *SHADER_VERT_GEOMETRY = R"###(#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUv;

layout(location = 0) out vec4 fPos;
layout(location = 1) out vec2 fUv;

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    mat4 mvp;
} globs;

layout(binding = 1) uniform sampler2D tex;

void main()
{
    fPos = (globs.mvp * vec4(vPosition, 1));
    fUv = vUv;

    gl_Position = fPos;
}
)###";

static const char *SHADER_FRAG_GEOMETRY = R"###(#version 460

layout(location = 0) in vec4 fPos;
layout(location = 1) in vec2 fUv;

layout(location = 0) out vec4 oColor;

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    mat4 mvp;
} globs;

layout(binding = 1) uniform sampler2D tex;

void main() {
    oColor = texture(tex, fUv);
}
)###";

struct ShaderUniformBuffer {
    Mat4f mvp;
};

class TestPass : public FrameGraphPass {
public:
    SHARED_PROPERTY(TestPass, COLOR)

    void setup(FrameGraphBuilder &builder) override {
        if (!vertexBufferRes.assigned) {
            VertexBufferDesc desc;
            desc.size = mesh.vertices.size() * mesh.vertexLayout.getSize();
            vertexBufferRes = builder.createVertexBuffer(desc);

            VertexArrayObjectDesc oDesc;
            oDesc.vertexLayout = mesh.vertexLayout;
            vertexArrayObjectRes = builder.createVertexArrayObject(oDesc);

            builder.write(vertexBufferRes);
        }

        builder.persist(vertexBufferRes);
        builder.persist(vertexArrayObjectRes);
        builder.read(vertexBufferRes);
        builder.read(vertexArrayObjectRes);

        if (!pipelineRes.assigned) {
            auto vs = ShaderSource(SHADER_VERT_GEOMETRY, "main", xng::VERTEX, xng::GLSL_460, false);
            auto fs = ShaderSource(SHADER_FRAG_GEOMETRY, "main", xng::FRAGMENT, xng::GLSL_460, false);

            vs = vs.preprocess(builder.getShaderCompiler());
            fs = fs.preprocess(builder.getShaderCompiler());

            vsb = vs.compile(builder.getShaderCompiler());
            fsb = fs.compile(builder.getShaderCompiler());

            pipelineRes = builder.createPipeline(RenderPipelineDesc{
                    .shaders = {
                            {VERTEX,   vsb.getShader()},
                            {FRAGMENT, fsb.getShader()}
                    },
                    .bindings = {BIND_SHADER_BUFFER,
                                 BIND_TEXTURE_BUFFER},
                    .primitive = TRIANGLES,
                    .vertexLayout = mesh.vertexLayout,
                    .clearColor = true,
                    .clearDepth = true
            });
        }

        builder.persist(pipelineRes);
        builder.read(pipelineRes);

        passRes = builder.createRenderPass(
                RenderPassDesc{.numberOfColorAttachments = builder.getBackBufferDescription().numberOfColorAttachments,
                        .hasDepthStencilAttachment = builder.getBackBufferDescription().hasDepthStencilAttachment});

        builder.read(passRes);

        shaderBufferRes = builder.createShaderBuffer(ShaderBufferDesc{.size =  sizeof(ShaderUniformBuffer)});
        builder.read(shaderBufferRes);
        builder.write(shaderBufferRes);

        switch (tex) {
            default:
                gBufferPosition = builder.getSharedData().get<FrameGraphResource>(
                        GBufferPass::GEOMETRY_BUFFER_POSITION);
                builder.read(gBufferPosition);
                break;
            case 1:
                gBufferAlbedo = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_ALBEDO);
                builder.read(gBufferAlbedo);
                break;
            case 2:
                gBufferAmbient = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_AMBIENT);
                builder.read(gBufferAmbient);
                break;
            case 3:
                gBufferSpecular = builder.getSharedData().get<FrameGraphResource>(
                        GBufferPass::GEOMETRY_BUFFER_SPECULAR);
                builder.read(gBufferSpecular);
                break;
            case 4:
                gBufferDepth = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_DEPTH);
                builder.read(gBufferDepth);
                break;
        }

        screenRes = builder.getBackBuffer();
        builder.write(screenRes);
    }

    void execute(FrameGraphPassResources &resources) override {
        auto &target = resources.get<RenderTarget>(screenRes);

        auto &pipeline = resources.get<RenderPipeline>(pipelineRes);
        auto &pass = resources.get<RenderPass>(passRes);

        auto &vertexBuffer = resources.get<VertexBuffer>(vertexBufferRes);
        auto &vertexArrayObject = resources.get<VertexArrayObject>(vertexArrayObjectRes);

        auto &shaderBuffer = resources.get<ShaderBuffer>(shaderBufferRes);

        if (!quadAllocated) {
            quadAllocated = true;
            auto verts = VertexStream().addVertices(mesh.vertices).getVertexBuffer();
            vertexBuffer.upload(0,
                                verts.data(),
                                verts.size());
            vertexArrayObject.bindBuffers(vertexBuffer);
        }

        ShaderUniformBuffer buf;
        buf.mvp = MatrixMath::identity();

        shaderBuffer.upload(buf);

        TextureBuffer *texture;
        switch (tex) {
            default:
                texture = &resources.get<TextureBuffer>(gBufferPosition);
                break;
            case 1:
                texture = &resources.get<TextureBuffer>(gBufferAlbedo);
                break;
            case 2:
                texture = &resources.get<TextureBuffer>(gBufferAmbient);
                break;
            case 3:
                texture = &resources.get<TextureBuffer>(gBufferSpecular);
                break;
            case 4:
                texture = &resources.get<TextureBuffer>(gBufferDepth);
                break;
        }

        pass.bindPipeline(pipeline);
        pass.bindVertexArrayObject(vertexArrayObject);
        pass.bindShaderData({
                                    shaderBuffer,
                                    *texture
                            });

        pass.beginRenderPass(target, {}, target.getDescription().size);
        pass.drawArray(RenderPass::DrawCall{.offset = 0, .count = mesh.vertices.size()});
        pass.endRenderPass();
    }

    std::type_index getTypeIndex() const override {
        return typeid(TestPass);
    }

    void setTex(int t) {
        tex = t;
    }

    void incrementTex() {
        if (++tex >= 4) {
            tex = 0;
        }
    }

    void decrementTex() {
        if (--tex < 0) {
            tex = 4;
        }
    }

private:
    Mesh mesh = Mesh::normalizedQuad();

    int tex = 0;

    FrameGraphResource screenRes;

    FrameGraphResource pipelineRes;
    FrameGraphResource passRes;

    FrameGraphResource shaderBufferRes;

    FrameGraphResource vertexBufferRes;
    FrameGraphResource vertexArrayObjectRes;

    SPIRVBundle vsb;
    SPIRVBundle fsb;

    bool quadAllocated = false;

    FrameGraphResource gBufferPosition;
    FrameGraphResource gBufferNormal;
    FrameGraphResource gBufferAlbedo;
    FrameGraphResource gBufferAmbient;
    FrameGraphResource gBufferSpecular;
    FrameGraphResource gBufferDepth;
};

#endif //XENGINE_TESTPASS_HPP
