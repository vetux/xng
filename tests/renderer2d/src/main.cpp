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

xng::ImageRGBA loadImage(const std::filesystem::path &filePath) {
    auto imageParser = xng::StbiParser();
    auto data = xng::readFile(filePath.string());
    return imageParser.read(data, filePath.extension(), nullptr).get<xng::ImageRGBA>();
}

int main(int argc, char *argv[]) {
    auto displayDriver = xng::DisplayDriver::load(xng::GLFW);
    auto gpuDriver = xng::GpuDriver::load(xng::OPENGL_4_6);
    auto shaderCompiler = xng::SPIRVCompiler::load(xng::SHADERC);
    auto shaderDecompiler = xng::SPIRVDecompiler::load(xng::SPIRV_CROSS);

    auto window = displayDriver->createWindow("opengl", "Renderer 2D Test", {640, 480}, {});
    auto &input = window->getInput();
    auto &target = window->getRenderTarget();

    auto renderDevice = gpuDriver->createRenderDevice();

    xng::Renderer2D ren(*renderDevice, *shaderCompiler, *shaderDecompiler);

    auto imageB = loadImage("assets/awesomeface.png");
    auto imageA = loadImage("assets/tux.png");

    auto texA = ren.createTexture(imageA);
    auto texB = ren.createTexture(imageB);

    auto frameLimiter = xng::FrameLimiter(60);

    float rotSpeed = 15;
    float rot = 0;

    while (!window->shouldClose()) {
        auto delta = frameLimiter.newFrame();

        rot += rotSpeed * delta;

        ren.renderBegin(target);

        auto targetSize = target.getDescription().size.convert<float>();

        ren.draw(xng::Vec2f(0, 0), targetSize, xng::ColorRGBA::green());

        ren.draw(xng::Vec2f(0, targetSize.y), xng::Vec2f(targetSize.x, 0), xng::ColorRGBA::green());

        ren.draw(xng::Rectf({}, imageA.getSize().convert<float>()),
                 xng::Rectf(targetSize / 2- imageA.getSize().convert<float>() / 2, imageA.getSize().convert<float>()),
                 texA,
                 {imageA.getSize().convert<float>() / 2},
                 rot,
                 0.25,
                 xng::ColorRGBA::cyan());

        ren.draw(xng::Rectf(targetSize / 2- imageA.getSize().convert<float>() / 2, imageA.getSize().convert<float>()),
                 xng::ColorRGBA::yellow(),
                 false,
                 {imageA.getSize().convert<float>() / 2},
                 rot);

        ren.draw(xng::Rectf({}, imageB.getSize().convert<float>()),
                 xng::Rectf({25, 25}, imageB.getSize().convert<float>()),
                 texB);

        ren.draw(xng::Rectf({25, 25}, imageB.getSize().convert<float>()),
                 xng::ColorRGBA::yellow(),
                 false);

        ren.renderPresent();

        window->swapBuffers();
        window->update();
    }

    ren.destroyTexture(texA);
    ren.destroyTexture(texB);

    return 0;
}