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

#ifndef XENGINE_DEBUGPASS_HPP
#define XENGINE_DEBUGPASS_HPP

#include "xng/xng.hpp"

#include "tests/debugpass_fs.hpp"
#include "tests/debugpass_vs.hpp"

using namespace xng;

struct ShaderData {
    float visualizeDepth_near_far[4];
};

/**
 * Display the selected slot texture in the back buffer.
 */
class DebugPass : public FrameGraphPass {
public:
    void setup(FrameGraphBuilder &builder) override {
        if (!vertexBufferRes.assigned) {
            VertexBufferDesc desc;
            desc.size = mesh.vertices.size() * mesh.vertexLayout.getSize();
            vertexBufferRes = builder.createVertexBuffer(desc);

            VertexArrayObjectDesc oDesc;
            oDesc.vertexLayout = mesh.vertexLayout;
            vertexArrayObjectRes = builder.createVertexArrayObject(oDesc);
        }

        builder.persist(vertexBufferRes);
        builder.persist(vertexArrayObjectRes);

        if (!pipelineRes.assigned) {
            pipelineRes = builder.createRenderPipeline(RenderPipelineDesc{
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

        builder.persist(pipelineRes);

        auto shaderBufferRes = builder.createShaderUniformBuffer(
                ShaderUniformBufferDesc{.size =  sizeof(::ShaderData)});

        auto displayTextureRes = builder.getSlot(tex);

        auto backBuffer = builder.getBackBuffer();

        auto camera = builder.getScene().rootNode.find<CameraProperty>().getProperty<CameraProperty>().camera;
        if (!quadAllocated) {
            quadAllocated = true;
            auto verts = VertexStream().addVertices(mesh.vertices).getVertexBuffer();
            builder.upload(vertexBufferRes, [verts]() {
                return FrameGraphCommand::UploadBuffer(verts.size(), verts.data());
            });
            builder.setVertexArrayObjectBuffers(vertexArrayObjectRes, vertexBufferRes, {}, {});
        }

        ::ShaderData buf{};
        buf.visualizeDepth_near_far[0] = tex == SLOT_DEFERRED_DEPTH
                                         || tex == SLOT_FORWARD_DEPTH
                                         || tex == SLOT_GBUFFER_DEPTH
                                         || tex == SLOT_SCREEN_DEPTH;
        buf.visualizeDepth_near_far[1] = camera.nearClip;
        buf.visualizeDepth_near_far[2] = 100;

        builder.upload(shaderBufferRes, [buf]() {
            return FrameGraphCommand::UploadBuffer(sizeof(buf), reinterpret_cast<const uint8_t *>(&buf));
        });

        builder.beginPass(backBuffer);
        builder.setViewport({}, builder.getBackBufferDescription().size);

        builder.renderClear(ColorRGBA(0), 1);

        builder.bindPipeline(pipelineRes);
        builder.bindVertexArrayObject(vertexArrayObjectRes);
        builder.bindShaderResources({
                                            {shaderBufferRes,
                                                                {{FRAGMENT, ShaderResource::READ}}},
                                            {displayTextureRes, {{FRAGMENT, ShaderResource::READ}}}

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

    FrameGraphResource pipelineRes;

    FrameGraphResource vertexBufferRes;
    FrameGraphResource vertexArrayObjectRes;

    bool quadAllocated = false;
};

#endif //XENGINE_DEBUGPASS_HPP
