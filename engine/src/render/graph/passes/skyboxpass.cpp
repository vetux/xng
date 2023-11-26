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

#include "xng/render/graph/passes/skyboxpass.hpp"

#include "xng/render/graph/framegraphbuilder.hpp"

#include "graph/skyboxpass_vs.hpp"
#include "graph/skyboxpass_fs.hpp"

#include "xng/render/graph/framegraphsettings.hpp"

#include "xng/render/geometry/vertexstream.hpp"

namespace xng {
    SkyboxPass::SkyboxPass() {

    }

    void xng::SkyboxPass::setup(FrameGraphBuilder &builder) {
        if (!pipeline.assigned) {
            pipeline = builder.createRenderPipeline(RenderPipelineDesc{
                    .shaders = {{VERTEX,   skyboxpass_vs},
                                {FRAGMENT, skyboxpass_fs}},
                    .bindings = {
                            BIND_SHADER_STORAGE_BUFFER,
                            BIND_TEXTURE_BUFFER,
                    },
                    .vertexLayout = cube.vertexLayout,
                    .enableDepthTest = false,
                    .enableFaceCulling = true,
                    .faceCullMode = CULL_FRONT,
                    .enableBlending = false
            });
        }
        builder.read(pipeline);
        builder.persist(pipeline);

        pass = builder.createRenderPass(RenderPassDesc{
                .numberOfColorAttachments = 1,
                .hasDepthStencilAttachment = true
        });
        builder.read(pass);

        auto renderSize = builder.getBackBufferDescription().size *
                          builder.getSettings().get<float>(FrameGraphSettings::SETTING_RENDER_SCALE);

        target = builder.createRenderTarget(RenderTargetDesc{
                .size = renderSize,
                .numberOfColorAttachments = 1,
                .hasDepthStencilAttachment = true,
        });
        builder.read(target);

        if (!vbAlloc) {
            vertexBuffer = builder.createVertexBuffer(VertexBufferDesc{
                    .size = cube.vertices.size() * cube.vertexLayout.getSize(),
            });

            indexBuffer = builder.createIndexBuffer(IndexBufferDesc{
                    .size = cube.indices.size() * sizeof(unsigned int),
            });

            vertexArrayObject = builder.createVertexArrayObject(VertexArrayObjectDesc{
                    .vertexLayout = cube.vertexLayout
            });
        }

        builder.write(vertexBuffer);
        builder.read(vertexBuffer);
        builder.persist(vertexBuffer);

        builder.write(indexBuffer);
        builder.read(indexBuffer);
        builder.persist(indexBuffer);

        builder.read(vertexArrayObject);
        builder.persist(vertexArrayObject);

        backgroundColor = builder.getSlot(SLOT_BACKGROUND_COLOR);
        builder.write(backgroundColor);

        uploadTexture = false;

        auto nodes = builder.getScene().rootNode.findAll({typeid(SkyboxProperty)});
        if (!nodes.empty()) {
            auto nskybox = nodes.at(0).getProperty<SkyboxProperty>().skybox;
            if (nskybox.texture.assigned()) {
                if (skybox.texture != nskybox.texture){
                    skyboxTexture = builder.createTextureBuffer(nskybox.texture.get().description);
                    uploadTexture = true;
                }
                builder.persist(skyboxTexture);
            } else {
                TextureBufferDesc desc;
                desc.textureType = TEXTURE_CUBE_MAP;
                skyboxTexture = builder.createTextureBuffer(desc);
            }
            skybox = nskybox;
        } else {
            skybox = {};
            TextureBufferDesc desc;
            desc.textureType = TEXTURE_CUBE_MAP;
            skyboxTexture = builder.createTextureBuffer(desc);
        }

        builder.read(skyboxTexture);

        cameraTransform = {};

        nodes = builder.getScene().rootNode.findAll({typeid(CameraProperty)});
        if (!nodes.empty()) {
            camera = nodes.at(0).getProperty<CameraProperty>().camera;
            if (nodes.at(0).hasProperty<TransformProperty>()){
                cameraTransform.setRotation(nodes.at(0).getProperty<TransformProperty>().transform.getRotation());
            }
        } else {
            camera = {};
        }

        commandBuffer = builder.createCommandBuffer();
        builder.write(commandBuffer);

        TextureBufferDesc desc;
        desc.textureType = TEXTURE_2D;
        desc.size = renderSize;
        desc.format = DEPTH_STENCIL;
        depthTex = builder.createTextureBuffer(desc);
        builder.write(depthTex);

        shaderBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(Mat4f)
        });
        builder.write(shaderBuffer);
    }

    void xng::SkyboxPass::execute(FrameGraphPassResources &resources,
                                  const std::vector<std::reference_wrapper<CommandQueue>> &renderQueues,
                                  const std::vector<std::reference_wrapper<CommandQueue>> &computeQueues,
                                  const std::vector<std::reference_wrapper<CommandQueue>> &transferQueues) {
        auto &com = resources.get<CommandBuffer>(commandBuffer);

        auto &pip = resources.get<RenderPipeline>(pipeline);
        auto &p = resources.get<RenderPass>(pass);
        auto &t = resources.get<RenderTarget>(target);

        auto &sb = resources.get<ShaderStorageBuffer>(shaderBuffer);

        auto &vb = resources.get<VertexBuffer>(vertexBuffer);
        auto &ib = resources.get<IndexBuffer>(indexBuffer);
        auto &vao = resources.get<VertexArrayObject>(vertexArrayObject);

        auto &bg = resources.get<TextureBuffer>(backgroundColor);
        auto &d = resources.get<TextureBuffer>(depthTex);

        auto &skyTex = resources.get<TextureBuffer>(skyboxTexture);

        if (uploadTexture){
            for (auto i = 0; i <= CubeMapFace::NEGATIVE_Z; i++){
                auto img = skybox.texture.get().images.at(static_cast<CubeMapFace>(i));
                skyTex.upload(static_cast<CubeMapFace>(i),
                              RGBA,
                              reinterpret_cast<const uint8_t *>(img.get().getData()),
                              img.get().getDataSize() * sizeof(ColorRGBA));
            }
        }

        if (!vbAlloc) {
            vbAlloc = true;
            VertexStream stream;
            stream.addVertices(cube.vertices);
            vb.upload(0,
                      reinterpret_cast<const uint8_t *>(stream.getVertexBuffer().data()),
                      stream.getVertexBuffer().size());
            ib.upload(0,
                      reinterpret_cast<const uint8_t *>(cube.indices.data()),
                      cube.indices.size() * sizeof(unsigned int));
            vao.setBuffers(vb, ib);
        }

        t.setAttachments({RenderTargetAttachment::texture(bg)}, RenderTargetAttachment::texture(d));

        sb.upload(camera.projection() * Camera::view(cameraTransform));

        std::vector<Command> commands;
        commands.emplace_back(p.begin(t));
        commands.emplace_back(p.clearColorAttachments(skybox.color));
        commands.emplace_back(pip.bind());
        commands.emplace_back(RenderPipeline::bindShaderResources({{
                                                                           sb, {{VERTEX, ShaderResource::READ},
                                                                                    {FRAGMENT, ShaderResource::READ}}
        },
                                                                   {
                                                                           skyTex, {{VERTEX, ShaderResource::READ},
                                                                                           {FRAGMENT, ShaderResource::READ}}
                                                                   }
        }));
        commands.emplace_back(vao.bind());
        commands.emplace_back(p.drawIndexed(DrawCall(0, cube.indices.size())));
        commands.emplace_back(p.end());

        com.begin();
        com.add(commands);
        com.end();

        renderQueues.at(0).get().submit(com);

        t.clearAttachments();
    }

    std::type_index xng::SkyboxPass::getTypeIndex() const {
        return typeid(xng::SkyboxPass);
    }
}