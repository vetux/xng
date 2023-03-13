/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2023  Julian Zampiccoli
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

static std::shared_ptr<EntityScene> createScene() {
    std::shared_ptr<EntityScene> scene = std::make_shared<EntityScene>();

    auto ent = scene->createEntity("canvas");

    CanvasComponent canvas;
    canvas.clearColor = ColorRGBA::grey(0.3);
    canvas.referenceResolution = {800, 600};
    canvas.scaleMode = xng::SCALE_REFERENCE_RESOLUTION;
    canvas.referenceFitWidth = 0.5;
    ent.createComponent(canvas);

    RectTransformComponent rect;
    rect.rectTransform.size = Vec2f(0);
    ent.createComponent(rect);

    ent = scene->createEntity("sprite");

    SpriteComponent sprite;
    sprite.sprite = ResourceHandle<Sprite>(Uri("sprites/tux.json"));
    ent.createComponent(sprite);

    rect = {};
    rect.parent = "canvas";
    rect.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_CENTER_CENTER;
    rect.rectTransform.size = sprite.sprite.get().image.get().getSize().convert<float>();
    ent.createComponent(rect);

    ent = scene->createEntity();

    rect = {};
    rect.parent = "canvas";
    rect.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_LEFT_TOP;
    rect.rectTransform.size = Vec2f(250, 50);
    ent.createComponent(rect);

    TextComponent text2;
    text2.text = "LEFT_TOP";
    text2.font = ResourceHandle<RawResource>(Uri("fonts/Sono/static/Sono/Sono-Bold.ttf"));
    text2.pixelSize.y = 30;
    text2.textColor = ColorRGBA::blue(1);
    ent.createComponent(text2);

    ent = scene->createEntity();

    rect = {};
    rect.parent = "canvas";
    rect.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_RIGHT_TOP;
    rect.rectTransform.size = Vec2f(250, 50);
    ent.createComponent(rect);

    TextComponent text3;
    text3.text = "RIGHT_TOP";
    text3.font = ResourceHandle<RawResource>(Uri("fonts/Sono/static/Sono/Sono-Bold.ttf"));
    text3.pixelSize.y = 30;
    text3.textColor = ColorRGBA::blue(1);
    ent.createComponent(text3);

    ent = scene->createEntity();

    rect = {};
    rect.parent = "canvas";
    rect.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_LEFT_BOTTOM;
    rect.rectTransform.size = Vec2f(250, 50);
    ent.createComponent(rect);

    TextComponent text4;
    text4.text = "LEFT_BOTTOM";
    text4.font = ResourceHandle<RawResource>(Uri("fonts/Sono/static/Sono/Sono-Bold.ttf"));
    text4.pixelSize.y = 30;
    text4.textColor = ColorRGBA::blue(1);
    ent.createComponent(text4);

    ent = scene->createEntity();

    rect = {};
    rect.parent = "canvas";
    rect.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_RIGHT_BOTTOM;
    rect.rectTransform.size = Vec2f(250, 50);
    ent.createComponent(rect);

    TextComponent text5;
    text5.text = "RIGHT_BOTTOM";
    text5.font = ResourceHandle<RawResource>(Uri("fonts/Sono/static/Sono/Sono-Bold.ttf"));
    text5.pixelSize.y = 30;
    text5.textColor = ColorRGBA::blue(1);
    ent.createComponent(text5);

    ent = scene->createEntity();

    rect = {};
    rect.parent = "canvas";
    rect.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_CENTER_CENTER;
    rect.rectTransform.size = Vec2f(480, 320);
    ent.createComponent(rect);

    ent = scene->createEntity();

    rect = {};
    rect.parent = "canvas";
    rect.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_CENTER_CENTER;
    rect.rectTransform.position = {};
    rect.rectTransform.size = Vec2f(800, 60);
    ent.createComponent(rect);

    TextComponent text6;
    text6.text = "Press Space Bar to change alignment...";
    text6.font = ResourceHandle<RawResource>(Uri("fonts/Sono/static/Sono/Sono-Bold.ttf"));
    text6.lineHeight = 50;
    text6.pixelSize.y = 30;
    text6.textColor = ColorRGBA::blue(1, 200);
    ent.createComponent(text6);

    return scene;
}

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

    auto scene = createScene();

    Renderer2D ren(*renderDevice, *shaderCompiler, *shaderDecompiler);

    SystemPipeline pipeline(xng::SystemPipeline::TICK_FRAME,
                            {
                                    std::make_shared<CanvasRenderSystem>(ren,
                                                                         target,
                                                                         *fontDriver,
                                                                         false)
                            });

    SystemRuntime runtime({pipeline}, scene);

    runtime.start();

    FrameLimiter limiter(60);
    while (!window->shouldClose()) {
        auto delta = limiter.newFrame();
        if (input.getDevice<Keyboard>().getKeyDown(xng::KEY_SPACE)) {
            auto spriteEnt = scene->getEntity("sprite");
            auto comp = spriteEnt.getComponent<RectTransformComponent>();
            if (comp.rectTransform.alignment == xng::RectTransform::RECT_ALIGN_RIGHT_BOTTOM) {
                comp.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_LEFT_TOP;
            } else {
                comp.rectTransform.alignment = static_cast<RectTransform::Alignment>(comp.rectTransform.alignment + 1);
            }
            spriteEnt.updateComponent(comp);
        }

        runtime.update(delta);
        window->update();
        window->swapBuffers();
    }

    runtime.stop();
}