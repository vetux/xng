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
        builder.persist(pipeline);

        auto renderSize = builder.getBackBufferDescription().size *
                          builder.getSettings().get<float>(FrameGraphSettings::SETTING_RENDER_SCALE);

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

        builder.persist(vertexBuffer);
        builder.persist(indexBuffer);
        builder.persist(vertexArrayObject);

        auto backgroundColor = builder.getSlot(SLOT_BACKGROUND_COLOR);

        uploadTexture = false;

        auto nodes = builder.getScene().rootNode.findAll({typeid(SkyboxProperty)});
        if (!nodes.empty()) {
            auto nskybox = nodes.at(0).getProperty<SkyboxProperty>().skybox;
            if (nskybox.texture.assigned()) {
                if (skybox.texture != nskybox.texture) {
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

        Camera camera;
        Transform cameraTransform;

        nodes = builder.getScene().rootNode.findAll({typeid(CameraProperty)});
        if (!nodes.empty()) {
            camera = nodes.at(0).getProperty<CameraProperty>().camera;
            if (nodes.at(0).hasProperty<TransformProperty>()) {
                cameraTransform.setRotation(nodes.at(0).getProperty<TransformProperty>().transform.getRotation());
            }
        } else {
            camera = {};
        }

        TextureBufferDesc desc;
        desc.textureType = TEXTURE_2D;
        desc.size = renderSize;
        desc.format = DEPTH_STENCIL;
        auto depthTex = builder.createTextureBuffer(desc);

        auto shaderBuffer = builder.createShaderStorageBuffer(ShaderStorageBufferDesc{
                .size = sizeof(Mat4f)
        });

        if (uploadTexture) {
            for (auto i = 0; i <= CubeMapFace::NEGATIVE_Z; i++) {
                auto img = skybox.texture.get().images.at(static_cast<CubeMapFace>(i));
                builder.upload(skyboxTexture, i, 0, [img]() {
                    return FrameGraphCommand::UploadBuffer(img.get().getDataSize() * sizeof(ColorRGBA),
                                                           reinterpret_cast<const uint8_t *>(img.get().getData()));
                });
            }
        }

        if (!vbAlloc) {
            vbAlloc = true;
            builder.upload(vertexBuffer,
                           [this]() {
                               VertexStream stream;
                               stream.addVertices(cube.vertices);
                               return FrameGraphCommand::UploadBuffer(stream.getVertexBuffer().size(),
                                                                      reinterpret_cast<const uint8_t *>(stream.getVertexBuffer().data()));
                           });
            builder.upload(indexBuffer,
                           [this]() {
                               return FrameGraphCommand::UploadBuffer(cube.indices.size() * sizeof(unsigned int),
                                                                      reinterpret_cast<const uint8_t *>(cube.indices.data()));
                           });

            builder.setVertexArrayObjectBuffers(vertexArrayObject, vertexBuffer, indexBuffer, {});
        }

        builder.upload(shaderBuffer,
                       [camera, cameraTransform]() {
                           auto mat = camera.projection() * Camera::view(cameraTransform);
                           return FrameGraphCommand::UploadBuffer(sizeof(Mat4f),
                                                                  reinterpret_cast<const uint8_t *>(mat.data));
                       });

        builder.clearTextureColor({backgroundColor}, skybox.color);

        builder.beginPass({FrameGraphCommand::Attachment::texture(backgroundColor)},
                          FrameGraphCommand::Attachment::texture(depthTex));
        builder.bindPipeline(pipeline);
        builder.bindShaderResources({{
                                             shaderBuffer,     {{VERTEX, ShaderResource::READ},
                                                             {FRAGMENT, ShaderResource::READ}}
                                     },
                                     {
                                             skyboxTexture, {{VERTEX, ShaderResource::READ},
                                                             {FRAGMENT, ShaderResource::READ}}
                                     }
                                    });
        builder.bindVertexArrayObject(vertexArrayObject);
        builder.drawIndexed(DrawCall(0, cube.indices.size()));
        builder.finishPass();
    }

    std::type_index xng::SkyboxPass::getTypeIndex() const {
        return typeid(xng::SkyboxPass);
    }
}