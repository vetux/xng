/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "xng/xng.hpp"

using namespace xng;

int main(int argc, char *argv[]) {
    auto displayDriver = DisplayDriver::load(DISPLAY_GLFW);
    auto gpuDriver = GpuDriver::load(OPENGL_4_6);
    auto shaderCompiler = ShaderCompiler::load(SHADERC);
    auto shaderDecompiler = ShaderDecompiler::load(SPIRV_CROSS);
    auto fontDriver = FontDriver::load(FREETYPE);

    auto window = displayDriver->createWindow(OPENGL_4_6, "Renderer 2D Test", {640, 480}, {});
    auto &input = window->getInput();
    auto &target = window->getRenderTarget();

    auto renderDevice = gpuDriver->createRenderDevice();

    ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>("assets/"));
    std::vector<std::unique_ptr<ResourceParser>> parsers;
    parsers.emplace_back(std::make_unique<StbiParser>());
    parsers.emplace_back(std::make_unique<JsonParser>());
    ResourceRegistry::getDefaultRegistry().setImporter(ResourceImporter(std::move(parsers)));

    std::shared_ptr<EntityScene> scene = std::make_shared<EntityScene>();

    auto ent = scene->createEntity("canvas");

    CanvasComponent canvas;
    canvas.clearColor = ColorRGBA::grey(0.3);
    ent.createComponent(canvas);

    ent = scene->createEntity("center");

    RectTransformComponent rect;
    rect.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_CENTER_CENTER;
    rect.rectTransform.size = Vec2f(1);
    rect.parent = "canvas";
    ent.createComponent(rect);

    ent = scene->createEntity("sprite");

    SpriteComponent sprite;
    sprite.sprite = ResourceHandle<Sprite>(Uri("sprites/tux.json"));
    ent.createComponent(sprite);
    rect = {};
    rect.parent = "center";
    rect.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_CENTER_CENTER;
    rect.rectTransform.size = sprite.sprite.get().image.get().getSize().convert<float>();
    ent.createComponent(rect);

    Renderer2D ren(*renderDevice, *shaderCompiler, *shaderDecompiler);

    SystemPipeline pipeline(xng::SystemPipeline::TICK_FRAME,
                            {
                                    std::make_shared<CanvasRenderSystem>(ren,
                                                                         target,
                                                                         *fontDriver)
                            });

    SystemRuntime runtime({pipeline}, scene);

    runtime.start();

    FrameLimiter limiter(60);
    while (!window->shouldClose()) {
        if (input.getDevice<Keyboard>().getKeyDown(xng::KEY_SPACE)) {
            auto spriteEnt = scene->getEntity("sprite");
            auto comp = spriteEnt.getComponent<RectTransformComponent>();
            if (comp.rectTransform.alignment == xng::RectTransform::RECT_ALIGN_RIGHT_BOTTOM){
                comp.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_LEFT_TOP;
            } else {
                comp.rectTransform.alignment = static_cast<RectTransform::Alignment>(comp.rectTransform.alignment + 1);
            }
            spriteEnt.updateComponent(comp);
        }
        auto centerEnt = scene->getEntity("center");
        auto comp = centerEnt.getComponent<RectTransformComponent>();
        comp.rectTransform.size = target.getDescription().size.convert<float>();
        centerEnt.updateComponent(comp);

        runtime.update(limiter.newFrame());
        window->update();
        window->swapBuffers();
    }

    runtime.stop();
}