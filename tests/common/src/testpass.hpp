/**
 *  xEngine - C++ Game Engine Library
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

#include "testpass/fs.hpp"
#include "testpass/vs.hpp"

using namespace xng;

struct ShaderData {
    float visualizeDepth_near_far[4];
};

class TestPass : public FrameGraphPass {
public:
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
            pipelineRes = builder.createPipeline(RenderPipelineDesc{
                    .shaders = {
                            {VERTEX,   vs},
                            {FRAGMENT, fs}
                    },
                    .bindings = {BIND_SHADER_UNIFORM_BUFFER,
                                 BIND_TEXTURE_BUFFER},
                    .primitive = TRIANGLES,
                    .vertexLayout = mesh.vertexLayout,
                    .enableBlending = false
            });
        }

        builder.persist(pipelineRes);
        builder.read(pipelineRes);

        passRes = builder.createRenderPass(
                RenderPassDesc{.numberOfColorAttachments = builder.getBackBufferDescription().numberOfColorAttachments,
                        .hasDepthStencilAttachment = builder.getBackBufferDescription().hasDepthStencilAttachment});

        builder.read(passRes);

        shaderBufferRes = builder.createShaderUniformBuffer(ShaderUniformBufferDesc{.size =  sizeof(::ShaderData)});
        builder.read(shaderBufferRes);
        builder.write(shaderBufferRes);

        switch (tex) {
            default:
                displayTextureRes = builder.getSlot(xng::SLOT_GBUFFER_POSITION);
                break;
            case 1:
                displayTextureRes = builder.getSlot(xng::SLOT_GBUFFER_NORMAL);
                break;
            case 2:
                displayTextureRes = builder.getSlot(xng::SLOT_GBUFFER_TANGENT);
                break;
            case 3:
                displayTextureRes = builder.getSlot(xng::SLOT_GBUFFER_ROUGHNESS_METALLIC_AO);
                break;
            case 4:
                displayTextureRes = builder.getSlot(xng::SLOT_GBUFFER_ALBEDO);
                break;
            case 5:
                displayTextureRes = builder.getSlot(xng::SLOT_GBUFFER_OBJECT_SHADOWS);
                break;
            case 6:
                displayTextureRes = builder.getSlot(xng::SLOT_GBUFFER_DEPTH);
                break;
            case 7:
                displayTextureRes = builder.getSlot(xng::SLOT_DEFERRED_COLOR);
                break;
            case 8:
                displayTextureRes = builder.getSlot(xng::SLOT_DEFERRED_DEPTH);
                break;
            case 9:
                displayTextureRes = builder.getSlot(xng::SLOT_FORWARD_COLOR);
                break;
            case 10:
                displayTextureRes = builder.getSlot(xng::SLOT_FORWARD_DEPTH);
                break;
            case 11:
                displayTextureRes = builder.getSlot(xng::SLOT_SCREEN_COLOR);
                break;
            case 12:
                displayTextureRes = builder.getSlot(xng::SLOT_SCREEN_DEPTH);
                break;
        }
        builder.read(displayTextureRes);

        backBuffer = builder.getBackBuffer();
        builder.write(backBuffer);

        camera = builder.getScene().rootNode.find<CameraProperty>().getProperty<CameraProperty>().camera;

        commandBuffer = builder.createCommandBuffer();
        builder.write(commandBuffer);
    }

    void
    execute(FrameGraphPassResources &resources, const std::vector<std::reference_wrapper<CommandQueue>> &renderQueues,
            const std::vector<std::reference_wrapper<CommandQueue>> &computeQueues,
            const std::vector<std::reference_wrapper<CommandQueue>> &transferQueues) override {
        auto &target = resources.get<RenderTarget>(backBuffer);

        auto &pipeline = resources.get<RenderPipeline>(pipelineRes);
        auto &pass = resources.get<RenderPass>(passRes);

        auto &vertexBuffer = resources.get<VertexBuffer>(vertexBufferRes);
        auto &vertexArrayObject = resources.get<VertexArrayObject>(vertexArrayObjectRes);

        auto &shaderBuffer = resources.get<ShaderUniformBuffer>(shaderBufferRes);

        auto &cBuffer = resources.get<CommandBuffer>(commandBuffer);

        if (!quadAllocated) {
            quadAllocated = true;
            auto verts = VertexStream().addVertices(mesh.vertices).getVertexBuffer();
            vertexBuffer.upload(0,
                                verts.data(),
                                verts.size());
            vertexArrayObject.setBuffers(vertexBuffer);
        }

        ::ShaderData buf{};
        buf.visualizeDepth_near_far[0] = tex == 6 || tex == 8 || tex == 10 || tex == 12;
        buf.visualizeDepth_near_far[1] = camera.nearClip;
        buf.visualizeDepth_near_far[2] = 100;

        shaderBuffer.upload(buf);

        auto &texture = resources.get<TextureBuffer>(displayTextureRes);

        cBuffer.begin();
        cBuffer.add(pass.begin(target));
        cBuffer.add(pass.setViewport({}, target.getDescription().size));
        cBuffer.add(pass.clearColorAttachments(ColorRGBA(0)));
        cBuffer.add(pass.clearDepthAttachment(1));
        cBuffer.add(pipeline.bind());
        cBuffer.add(vertexArrayObject.bind());
        cBuffer.add(RenderPipeline::bindShaderResources({
                                                                ShaderResource{shaderBuffer,
                                                                               {{FRAGMENT, ShaderResource::READ}}},
                                                                {texture, {{FRAGMENT, ShaderResource::READ}}}
                                                        }));
        cBuffer.add(pass.drawArray(DrawCall(0, mesh.vertices.size())));
        cBuffer.add(pass.end());
        renderQueues.at(0).get().submit(cBuffer);
    }

    std::type_index getTypeIndex() const override {
        return typeid(TestPass);
    }

    void setTex(int t) {
        tex = t;
    }

    int getTex() {
        return tex;
    }

    std::string getTexName() {
        std::string txt;
        switch (tex) {
            default:
                return "INVALID";
            case 0:
                return  "GBUFFER POSITION";
            case 1:
                return "GBUFFER NORMAL";
            case 2:
                return "GBUFFER TANGENT";
            case 3:
                return "GBUFFER ROUGHNESS_METALLIC_AO";
            case 4:
                return "GBUFFER ALBEDO";
            case 5:
                return "GBUFFER OBJECT_SHADOWS";
            case 6:
                return "GBUFFER DEPTH";
            case 7:
                return "DEFERRED COLOR";
            case 8:
                return "DEFERRED DEPTH";
            case 9:
                return "FORWARD COLOR";
            case 10:
                return "FORWARD DEPTH";
            case 11:
                return "SCREEN COLOR";
            case 12:
                return "SCREEN DEPTH";
        }
    }

    void incrementTex() {
        if (++tex > 12) {
            tex = 0;
        }
    }

    void decrementTex() {
        if (--tex < 0) {
            tex = 12;
        }
    }

private:
    Mesh mesh = Mesh::normalizedQuad();

    int tex = 11;

    FrameGraphResource backBuffer;

    FrameGraphResource pipelineRes;
    FrameGraphResource passRes;

    FrameGraphResource shaderBufferRes;

    FrameGraphResource vertexBufferRes;
    FrameGraphResource vertexArrayObjectRes;

    bool quadAllocated = false;

    FrameGraphResource displayTextureRes;

    FrameGraphResource commandBuffer;

    Camera camera;
};

#endif //XENGINE_TESTPASS_HPP
