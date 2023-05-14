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

#include <fstream>

using namespace xng;

static const int fpsFontPixelSize = 30;
static const int fpsFontPixelLineHeight = 25;
static const ColorRGBA fpsFontColor = ColorRGBA::fuchsia();

ImageRGBA loadImage(const std::filesystem::path &filePath) {
    auto imageParser = StbiParser();
    auto data = readFile(filePath.string());
    return imageParser.read(data, filePath.extension().string(), nullptr).get<ImageRGBA>();
}

class TestApplication {
public:
    RenderDevice &device;
    Window &window;
    Renderer2D &ren;

    ImageRGBA imageA;
    ImageRGBA imageB;

    TextureAtlasHandle texA;
    TextureAtlasHandle texB;

    float rotSpeed = 55;
    float rot{};

    float performanceGridBase = 2;

    TextRenderer textRenderer;

    TestApplication(RenderDevice &device,
                    Window &window,
                    Renderer2D &ren,
                    Font &font)
            : device(device),
            window(window),
              ren(ren),
              textRenderer(font, ren, Vec2i(0, fpsFontPixelSize)) {
        imageB = loadImage("assets/images/awesomeface.png");
        imageA = loadImage("assets/images/tux.png");

        texA = ren.createTexture(imageA);
        texB = ren.createTexture(imageB);
    }

    ~TestApplication() {
        ren.destroyTexture(texA);
        ren.destroyTexture(texB);
    }

    void drawHomePage(DeltaTime delta) {
        auto textLayout = TextLayout();
        textLayout.lineHeight = fpsFontPixelLineHeight;

        std::ostringstream out;
        out.precision(0);
        out << std::fixed << 1 / delta;

        auto text = textRenderer.render(out.str(), textLayout);
        auto &textImg = text.getImage();
        auto textHandle = ren.createTexture(textImg);

        auto target = window.getRenderTarget(device);

        rot += rotSpeed * delta;

        ren.renderBegin(*target, ColorRGBA::gray(0.3, 0));

        auto targetSize = target->getDescription().size.convert<float>();

        ren.draw(Vec2f(0, 0), targetSize, ColorRGBA::green());

        ren.draw(Vec2f(0, targetSize.y), Vec2f(targetSize.x, 0), ColorRGBA::green());

        ren.draw(Rectf({}, imageA.getSize().convert<float>()),
                 Rectf(targetSize / 2 - imageA.getSize().convert<float>() / 2, imageA.getSize().convert<float>()),
                 texA,
                 {imageA.getSize().convert<float>() / 2},
                 0,
                 xng::LINEAR,
                 ColorRGBA(255));

        ren.draw(Rectf(targetSize / 2 - imageA.getSize().convert<float>() / 2, imageA.getSize().convert<float>()),
                 ColorRGBA::yellow(),
                 false,
                 {imageA.getSize().convert<float>() / 2},
                 0);

        ren.draw(Rectf({}, imageB.getSize().convert<float>()),
                 Rectf({25, 25}, imageB.getSize().convert<float>()),
                 texB,
                 {},
                 0,
                 xng::LINEAR,
                 0,
                 0,
                 {});

        ren.draw(Rectf({25, 25}, imageB.getSize().convert<float>()),
                 ColorRGBA::yellow(),
                 false);

        ren.draw(Rectf({}, textImg.getSize().convert<float>()),
                 Rectf({}, textImg.getSize().convert<float>()),
                 textHandle,
                 {},
                 0,
                 xng::NEAREST,
                 fpsFontColor);

        ren.renderPresent();

        ren.destroyTexture(textHandle);
    }

    size_t drawPerformanceTest(DeltaTime delta) {
        auto textLayout = TextLayout();
        textLayout.lineHeight = fpsFontPixelLineHeight;

        std::ostringstream out;
        out.precision(0);
        out << std::fixed << 1 / delta;

        auto text = textRenderer.render(out.str(), textLayout);
        auto &textImg = text.getImage();
        auto textHandle = ren.createTexture(textImg);

        if (window.getInput().getDevice<Keyboard>().getKey(KEY_UP)) {
            performanceGridBase++;
        } else if (window.getInput().getDevice<Keyboard>().getKey(xng::KEY_DOWN)) {
            performanceGridBase--;
            if (performanceGridBase < 0) {
                performanceGridBase = 0;
            }
        }

        auto target = window.getRenderTarget(device);

        rot += rotSpeed * delta;

        ren.renderBegin(*target, ColorRGBA::gray(0.3, 0));

        auto targetSize = target->getDescription().size.convert<float>();

        auto performanceGrid = Vec2f(performanceGridBase * (targetSize.x / targetSize.y), performanceGridBase);

        auto padding = 15.0f;
        auto spacing = 5.0f;

        auto texSize = (targetSize - (Vec2f(spacing) * performanceGrid.convert<float>() + (Vec2f(padding) * 2)))
                       / performanceGrid.convert<float>();

        float imgAp = static_cast<float>(imageA.getWidth()) / static_cast<float>(imageA.getHeight());

        auto width = texSize.x;
        auto widthDiff = 0.f;

        auto height = width * imgAp;
        auto heightDiff = texSize.y - height;

        if (heightDiff < 0) {
            width = width - ((height - texSize.y) / imgAp);
            widthDiff = texSize.x - width;
            height = width * imgAp;
            heightDiff = texSize.y - height;
        }

        auto imgSize = Vec2f(height, width);

        auto ret = 0;
        for (int x = 0; x < performanceGrid.x; x++) {
            for (int y = 0; y < performanceGrid.y; y++) {
                Vec2f pos = Vec2f(padding)
                            + (Vec2f(static_cast<float>(x), static_cast<float>(y)) * Vec2f(spacing))
                            + (Vec2f(static_cast<float>(x), static_cast<float>(y)) * texSize)
                            + Vec2f(widthDiff / 2, heightDiff / 2);
                Vec2f scale = Vec2i(x, y).convert<float>() / performanceGrid.convert<float>();
                ren.draw(Rectf({}, imageA.getSize().convert<float>()),
                         Rectf(pos, imgSize),
                         texA,
                         imgSize / 2,
                         rot,
                         xng::LINEAR,
                         std::clamp((scale.x + scale.y) - 0.5f, 0.0f, 1.0f),
                         0,
                         xng::ColorRGBA(static_cast<uint8_t>(125 * scale.x + 25),
                                        static_cast<uint8_t>(125 * scale.y + 25), 30, 255));
                ret++;
            }
        }

        ren.draw(Rectf({}, textImg.getSize().convert<float>()),
                 Rectf({}, textImg.getSize().convert<float>()),
                 textHandle,
                 {},
                 0,
                 xng::NEAREST,
                 fpsFontColor);

        ren.renderPresent();

        ren.destroyTexture(textHandle);

        return ret;
    }
};

//TODO: Fix textures beeing drawn as black color on windows.
int main(int argc, char *argv[]) {
    auto displayDriver = glfw::GLFWDisplayDriver();
    auto gpuDriver = opengl::OGLGpuDriver();
    auto shaderCompiler = shaderc::ShaderCCompiler();
    auto shaderDecompiler = spirv_cross::SpirvCrossDecompiler();
    auto fontDriver = freetype::FtFontDriver();

    auto window = displayDriver.createWindow(OPENGL_4_6, "Renderer 2D Test", {640, 480}, {.swapInterval = 1});

    auto renderDevice = gpuDriver.createRenderDevice();

    auto &input = window->getInput();
    auto target = window->getRenderTarget(*renderDevice);
    auto fs = std::ifstream("assets/fonts/Sono/static/Sono/Sono-Bold.ttf", std::ios_base::in | std::ios::binary);
    auto font = fontDriver.createFont(fs);

    font->setPixelSize({0, fpsFontPixelSize});

    Renderer2D ren(*renderDevice, shaderCompiler, shaderDecompiler);

    auto frameLimiter = FrameLimiter();

    TestApplication app(*renderDevice, *window, ren, *font);

    size_t currentPage = 0;
    while (!window->shouldClose()) {
        auto delta = frameLimiter.newFrame();

        if (input.getDevice<Keyboard>().getKeyDown(KeyboardKey::KEY_1)) {
            currentPage = 0;
        } else if (input.getDevice<Keyboard>().getKeyDown(KeyboardKey::KEY_2)) {
            currentPage = 1;
        }

        switch (currentPage) {
            default: {
                app.drawHomePage(delta);
                auto msg = "Drawing Home Page " + std::to_string(ren.getPolyDrawCount()) + " Polys, " +
                           std::to_string(1.0f / delta) + " fps.";
                break;
            }
            case 1: {
                auto c = app.drawPerformanceTest(delta);
                auto msg = "Drawing Performance Test " + std::to_string(ren.getPolyDrawCount()) + " Polys " +
                           std::to_string(c) + " Sprites, " + std::to_string(1.0f / delta) + " fps.";
                break;
            }
        }

        window->update();
        window->swapBuffers();
    }

    return 0;
}
