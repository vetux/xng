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

#include "xng/xng.hpp"

using namespace xng;

static const char *SHADER_VS = R"###(
#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUv;

layout (location = 0) out vec4 fPosition;
layout (location = 1) out vec2 fUv;
layout (location = 2) flat out uint drawID;

struct ShaderUniformData {
    mat4 mvp;
};

layout(binding = 0, std140) buffer ShaderUniformBuffer
{
    ShaderUniformData data[];
} globs;

layout(binding = 1) uniform sampler2D texture2D;
layout(binding = 2) uniform sampler2DArray texture2DArray;

void main() {
    fPosition =  globs.data[gl_DrawID].mvp * vec4(vPosition, 1);
    fUv = vUv;
    fUv.y = 1 - fUv.y;
    drawID = gl_DrawID;
    gl_Position = fPosition;
}

)###";

static const char *SHADER_FS = R"###(
#version 460

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;
layout (location = 2) flat in uint drawID;

layout (location = 0) out vec4 color;

struct ShaderUniformData {
    mat4 mvp;
};

layout(binding = 0, std140) buffer ShaderUniformBuffer
{
    ShaderUniformData data[];
} globs;

layout(binding = 1) uniform sampler2D texture2D;
layout(binding = 2) uniform sampler2DArray texture2DArray;

void main() {
    vec4 texColor = texture(texture2D, fUv);
    vec4 texArrayColor = texture(texture2DArray, vec3(fUv.x, fUv.y, 0));
    color = texArrayColor;
}

)###";

#pragma pack(push, 1)
struct ShaderDrawData {
    Mat4f mvp;
};
#pragma pack(pop)

int main(int argc, char *argv[]) {
    auto displayDriver = glfw::GLFWDisplayDriver();
    auto gpuDriver = opengl::OGLGpuDriver();
    auto shaderCompiler = shaderc::ShaderCCompiler();
    auto shaderDecompiler = spirv_cross::SpirvCrossDecompiler();
    auto imageParser = StbiParser();

    auto window = displayDriver.createWindow(xng::OPENGL_4_6,
                                             "Shader Test",
                                             {640, 480},
                                             {.swapInterval = 1,
                                                     .debug = true});

    auto device = gpuDriver.createRenderDevice();

    device->setDebugCallback([](const std::string &msg) {
        std::cout << msg << "\n";
    });

    auto vsB = SPIRVBundle(std::vector<SPIRVBundle::Entry>{
                                   SPIRVBundle::Entry{
                                           .stage = xng::VERTEX,
                                           .entryPoint = "main",
                                           .blobIndex = 0}
                           },
                           {shaderCompiler.compile(SHADER_VS,
                                                   "main",
                                                   VERTEX,
                                                   GLSL_460,
                                                   ShaderCompiler::OPTIMIZATION_NONE)});
    auto fsB = SPIRVBundle(std::vector<SPIRVBundle::Entry>{
                                   SPIRVBundle::Entry{
                                           .stage = xng::FRAGMENT,
                                           .entryPoint = "main",
                                           .blobIndex = 0}
                           },
                           {shaderCompiler.compile(SHADER_FS,
                                                   "main",
                                                   FRAGMENT,
                                                   GLSL_460,
                                                   ShaderCompiler::OPTIMIZATION_NONE)});

    auto mesh = Mesh::normalizedQuad();

    auto vertexStream = VertexStream().addVertices(mesh.vertices);

    auto pipeline = device->createRenderPipeline(RenderPipelineDesc{
                                                         .shaders = {
                                                                 {VERTEX,   vsB.getShader()},
                                                                 {FRAGMENT, fsB.getShader()}
                                                         },
                                                         .bindings = {
                                                                 BIND_SHADER_STORAGE_BUFFER,
                                                                 BIND_TEXTURE_BUFFER,
                                                                 BIND_TEXTURE_ARRAY_BUFFER
                                                         },
                                                         .vertexLayout = mesh.vertexLayout,
                                                         .clearColor = true,
                                                 },
                                                 shaderDecompiler);

    auto vertexBuffer = device->createVertexBuffer(VertexBufferDesc{
            .bufferType = RenderBufferType::HOST_VISIBLE,
            .size = mesh.vertices.size() * mesh.vertexLayout.getSize()
    });

    vertexBuffer->upload(0,
                         reinterpret_cast<const uint8_t *>(vertexStream.getVertexBuffer().data()),
                         vertexStream.getVertexBuffer().size());

    auto vertexArray = device->createVertexArrayObject(VertexArrayObjectDesc{
            .vertexLayout = mesh.vertexLayout
    });

    vertexArray->bindBuffers(*vertexBuffer);

    auto pass = device->createRenderPass(RenderPassDesc{
            .numberOfColorAttachments = 1,
            .hasDepthStencilAttachment = false
    });

    auto target = window->getRenderTarget(*device);

    auto bundle = imageParser.read(readFile("assets/images/awesomeface.png"), "png", nullptr);

    auto &image = bundle.get<ImageRGBA>();

    TextureBufferDesc desc;
    desc.size = image.getSize();
    desc.textureType = TEXTURE_2D;

    TextureArrayBufferDesc aDesc;
    aDesc.textureDesc = desc;
    aDesc.textureCount = 1;

    auto textureBuffer = device->createTextureBuffer(desc);
    auto textureArrayBuffer = device->createTextureArrayBuffer(aDesc);

    textureBuffer->upload(image);
    textureArrayBuffer->upload(0, image);

    auto shaderBuffer = device->createShaderStorageBuffer(ShaderStorageBufferDesc{
            .size = sizeof(ShaderDrawData)
    });

    shaderBuffer->upload(ShaderDrawData{.mvp = MatrixMath::identity()});

    while (!window->shouldClose()) {
        pass->beginRenderPass(*target, {}, target->getDescription().size);
        pass->bindVertexArrayObject(*vertexArray);
        pass->bindShaderData({
                                     *shaderBuffer,
                                     *textureBuffer,
                                     *textureArrayBuffer
                             });
        pass->bindPipeline(*pipeline);
        RenderPass::DrawCall drawCall;
        drawCall.count = mesh.vertices.size();
        pass->multiDrawArray({drawCall});
        pass->endRenderPass();
        window->update();
        window->swapBuffers();
    }
}