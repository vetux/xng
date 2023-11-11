/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/xng.hpp"

#include "debugoverlay.hpp"
#include "tests/mandelbrot.hpp"

using namespace xng;

#pragma pack(push, 1)

struct MandelBrotData {
    double cx_cy_scale[4];
    int maxIterationsColorMode[4];
    float color[4];
};

#pragma pack(pop)

class MandelbrotRenderer {
public:
    RenderDevice &device;

    std::unique_ptr<ComputePipeline> pipeline;
    std::unique_ptr<CommandBuffer> commandBuffer;

    std::unique_ptr<TextureBuffer> texture;
    std::unique_ptr<ShaderUniformBuffer> uniformBuffer;

    explicit MandelbrotRenderer(RenderDevice &device, ShaderDecompiler &decompiler) : device(device) {
        commandBuffer = device.createCommandBuffer();
        ComputePipelineDesc desc;
        desc.bindings = {
                BIND_SHADER_UNIFORM_BUFFER,
                BIND_IMAGE_BUFFER
        };
        desc.shaders = {{COMPUTE, mandelbrot}};
        pipeline = device.createComputePipeline(desc, decompiler);
    }

    void renderMandelbrot(MandelBrotData &data, const Vec2i &size) {
        if (!texture || texture->getDescription().size != size) {
            TextureBufferDesc desc;
            desc.size = size;
            texture = device.createTextureBuffer(desc);
        }
        if (!uniformBuffer) {
            ShaderUniformBufferDesc desc;
            desc.size = sizeof(MandelBrotData);
            uniformBuffer = device.createShaderUniformBuffer(desc);
        }
        uniformBuffer->upload(data);
        commandBuffer->begin();
        commandBuffer->add(pipeline->bind());
        commandBuffer->add(ComputePipeline::bindShaderResources({
                                                                        {*uniformBuffer, {{COMPUTE, ShaderResource::WRITE}}},
                                                                        {*texture,       {{COMPUTE, ShaderResource::WRITE}}}
                                                                }));
        commandBuffer->add(ComputePipeline::execute({static_cast<unsigned int>(size.x),
                                                     static_cast<unsigned int>(size.y),
                                                     1}));
        commandBuffer->end();
        device.getComputeCommandQueues().at(0).get().submit(*commandBuffer);
    }
};

int main(int argc, char *argv[]) {
    opengl::OGLGpuDriver gpuDriver;
    glfw::GLFWDisplayDriver displayDriver;
    freetype::FtFontDriver fontDriver;
    glslang::GLSLangCompiler shaderCompiler;
    spirv_cross::SpirvCrossDecompiler shaderDecompiler;

    std::unique_ptr<Window> window = displayDriver.createWindow(xng::OPENGL_4_6, "Mandelbrot",
                                                                {640, 480},
                                                                {});
    std::unique_ptr<RenderDevice> device = gpuDriver.createRenderDevice();
    std::unique_ptr<RenderTarget> screen = window->getRenderTarget(*device);

    auto fs = std::ifstream("assets/fonts/Sono/static/Sono/Sono-Bold.ttf", std::ios_base::in | std::ios::binary);
    auto font = fontDriver.createFont(fs);

    Renderer2D ren2D(*device, shaderCompiler, shaderDecompiler);

    DebugOverlay debugOverlay(*font, ren2D);

    MandelbrotRenderer mandelbrotRenderer(*device, shaderDecompiler);

    FrameLimiter limiter;

    std::unique_ptr<RenderTarget> blitTarget;
    std::unique_ptr<CommandBuffer> commandBuffer = device->createCommandBuffer();
    MandelBrotData data{};
    data.cx_cy_scale[0] = 0;
    data.cx_cy_scale[1] = 0;
    data.cx_cy_scale[2] = 0.0018;
    data.maxIterationsColorMode[0] = 20;
    data.maxIterationsColorMode[1] = 0;

    auto color = ColorRGBA::white().divide();
    data.color[0] = color.x;
    data.color[1] = color.y;
    data.color[2] = color.z;
    data.color[3] = color.w;
    while (!window->shouldClose()) {
        auto delta = limiter.newFrame();

        Vec2i move;
        if (window->getInput().getKeyboard().getKey(KEY_W)) {
            move.y = 1;
        } else if (window->getInput().getKeyboard().getKey(KEY_S)) {
            move.y = -1;
        }
        if (window->getInput().getKeyboard().getKey(KEY_A)) {
            move.x = -1;
        } else if (window->getInput().getKeyboard().getKey(xng::KEY_D)) {
            move.x = 1;
        }

        float scale = 0;
        if (window->getInput().getKeyboard().getKey(KEY_UP)) {
            scale = 1;
        } else if (window->getInput().getKeyboard().getKey(KEY_DOWN)) {
            scale = -1;
        }

        float iteration = 0;
        if (window->getInput().getKeyboard().getKey(KEY_LEFT)) {
            iteration = 1;
        } else if (window->getInput().getKeyboard().getKey(xng::KEY_RIGHT)) {
            iteration = -1;
        }

        if (window->getInput().getKeyboard().getKeyDown(xng::KEY_C)){
            data.maxIterationsColorMode[1] += 1;
            if (data.maxIterationsColorMode[1] > 1){
                data.maxIterationsColorMode[1] = 0;
            }
        }

        float movementSpeed = 50;

        data.cx_cy_scale[0] += move.x * delta * data.cx_cy_scale[2] * movementSpeed;
        data.cx_cy_scale[1] += move.y * delta * data.cx_cy_scale[2] * movementSpeed;
        data.cx_cy_scale[2] += scale * delta * data.cx_cy_scale[2];

        data.maxIterationsColorMode[0] += iteration;

        mandelbrotRenderer.renderMandelbrot(data, screen->getDescription().size);

        if (!blitTarget || blitTarget->getDescription().size != screen->getDescription().size) {
            RenderTargetDesc desc = screen->getDescription();
            desc.hasDepthStencilAttachment = false;
            blitTarget = device->createRenderTarget(screen->getDescription());
        }

        blitTarget->setAttachments({RenderTargetAttachment::texture(*mandelbrotRenderer.texture)});

        commandBuffer->begin();
        commandBuffer->add(
                screen->blitColor(*blitTarget, {}, {}, blitTarget->getDescription().size, screen->getDescription().size,
                                  NEAREST, 0, 0));
        commandBuffer->end();

        device->getRenderCommandQueues().at(0).get().submit(*commandBuffer);

        debugOverlay.draw(delta, *screen,
                          std::to_string(data.cx_cy_scale[0]) + "," +
                          std::to_string(data.cx_cy_scale[1]) + " " +
                          std::to_string(data.cx_cy_scale[2]) + "\n"
                          + std::to_string(data.maxIterationsColorMode[0]));

        window->swapBuffers();
        window->update();
    }
}