/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_DEBUGPASS_HPP
#define XENGINE_DEBUGPASS_HPP

#include "xng/xng.hpp"

#include "tests/debugpass_fs.hpp"
#include "tests/debugpass_vs.hpp"

using namespace xng;

struct DebugShaderData {
    float visualizeDepth_near_far[4];
};

/**
 * Display the selected slot texture in the back buffer.
 */
class DebugPass : public FrameGraphPass {
public:
    void setup(FrameGraphBuilder &builder) override {
        if (!vertexBuffer.assigned) {
            VertexBufferDesc desc;
            desc.size = mesh.vertices.size() * mesh.vertexLayout.getLayoutSize();
            vertexBuffer = builder.createVertexBuffer(desc);

            builder.upload(vertexBuffer, [this]() {
                return FrameGraphUploadBuffer::createArray(VertexStream().addVertices(mesh.vertices).getVertexBuffer());
            });
        }

        builder.persist(vertexBuffer);

        if (!pipeline.assigned) {
            pipeline = builder.createRenderPipeline(RenderPipelineDesc{
                    .shaders = {
                            {VERTEX,   debugpass_vs},
                            {FRAGMENT, debugpass_fs}
                    },
                    .bindings = {BIND_SHADER_UNIFORM_BUFFER,
                                 BIND_TEXTURE_BUFFER},
                    .primitive = TRIANGLES,
                    .vertexLayout = mesh.vertexLayout,
                    .enableBlending = false
            });
        }

        builder.persist(pipeline);

        auto shaderBuffer = builder.createShaderUniformBuffer(
                ShaderUniformBufferDesc{.size =  sizeof(::ShaderData)});

        auto displayTexture = builder.getSlot(tex);

        auto backBuffer = builder.getBackBuffer();

        auto camera = builder.getScene().rootNode.find<CameraProperty>().getProperty<CameraProperty>().camera;

        DebugShaderData buf{};
        buf.visualizeDepth_near_far[0] = tex == SLOT_DEFERRED_DEPTH
                                         || tex == SLOT_FORWARD_DEPTH
                                         || tex == SLOT_GBUFFER_DEPTH
                                         || tex == SLOT_SCREEN_DEPTH;
        buf.visualizeDepth_near_far[1] = camera.nearClip;
        buf.visualizeDepth_near_far[2] = 100;

        builder.upload(shaderBuffer, [buf]() {
            return FrameGraphUploadBuffer::createValue(buf);
        });

        builder.beginPass(backBuffer);
        builder.setViewport({}, builder.getBackBufferDescription().size);

        builder.clearColor(ColorRGBA(0));
        builder.clearDepth(1);

        builder.bindPipeline(pipeline);
        builder.bindVertexBuffers(vertexBuffer, {}, {}, mesh.vertexLayout, {});
        builder.bindShaderResources({
                                            {shaderBuffer,      {{FRAGMENT, ShaderResource::READ}}},
                                            {displayTexture, {{FRAGMENT, ShaderResource::READ}}}

                                    });
        builder.drawArray(DrawCall(0, mesh.vertices.size()));
        builder.finishPass();
    }

    std::type_index getTypeIndex() const override {
        return typeid(DebugPass);
    }

    void setSlot(FrameGraphSlot t) {
        tex = t;
    }

    FrameGraphSlot getSlot() {
        return tex;
    }

    std::string getSlotName() {
        std::string txt;
        switch (tex) {
            default:
                return "INVALID";
            case SLOT_GBUFFER_POSITION:
                return "GBUFFER POSITION";
            case SLOT_GBUFFER_NORMAL:
                return "GBUFFER NORMAL";
            case SLOT_GBUFFER_TANGENT:
                return "GBUFFER TANGENT";
            case SLOT_GBUFFER_ROUGHNESS_METALLIC_AO:
                return "GBUFFER ROUGHNESS_METALLIC_AO";
            case SLOT_GBUFFER_ALBEDO:
                return "GBUFFER ALBEDO";
            case SLOT_GBUFFER_OBJECT_SHADOWS:
                return "GBUFFER OBJECT_SHADOWS";
            case SLOT_GBUFFER_DEPTH:
                return "GBUFFER DEPTH";
            case SLOT_DEFERRED_COLOR:
                return "DEFERRED COLOR";
            case SLOT_DEFERRED_DEPTH:
                return "DEFERRED DEPTH";
            case SLOT_FORWARD_COLOR:
                return "FORWARD COLOR";
            case SLOT_FORWARD_DEPTH:
                return "FORWARD DEPTH";
            case SLOT_BACKGROUND_COLOR:
                return "BACKGROUND COLOR";
            case SLOT_SCREEN_COLOR:
                return "SCREEN COLOR";
            case SLOT_SCREEN_DEPTH:
                return "SCREEN DEPTH";
        }
    }

    void incrementSlot() {
        if (tex == SLOT_GBUFFER_DEPTH) {
            tex = SLOT_SCREEN_COLOR;
        } else {
            tex = static_cast<FrameGraphSlot>(tex + 1);
        }
    }

    void decrementSlot() {
        if (tex == SLOT_SCREEN_COLOR) {
            tex = SLOT_GBUFFER_DEPTH;
        } else {
            tex = static_cast<FrameGraphSlot>(tex - 1);
        }
    }

private:
    Mesh mesh = Mesh::normalizedQuad();

    FrameGraphSlot tex = SLOT_SCREEN_COLOR;

    FrameGraphResource pipeline;
    FrameGraphResource vertexBuffer;

    bool quadAllocated = false;
};

#endif //XENGINE_DEBUGPASS_HPP
