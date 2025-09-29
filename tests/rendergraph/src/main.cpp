/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/adapters/glfw/glfw.hpp"
#include "xng/adapters/opengl/opengl.hpp"
#include "xng/adapters/freetype/freetype.hpp"
#include "xng/adapters/assimp/assimp.hpp"

using namespace xng;

RenderScene createScene() {
    // Scene interface will be redesign next.
    RenderScene scene;

    SkinnedMeshObject skinnedMesh;
    skinnedMesh.transform = Transform(Vec3f(0, 0, -20), Vec3f(), Vec3f(1, 1, 1));
    skinnedMesh.mesh = ResourceHandle<SkinnedMesh>(Uri("file://meshes/sphere.obj"));

    scene.skinnedMeshes.push_back(skinnedMesh);

    return scene;
}

int main(int argc, char *argv[]) {
    std::vector<std::unique_ptr<ResourceImporter> > importers;
    importers.emplace_back(std::make_unique<FontImporter>());
    importers.emplace_back(std::make_unique<StbiImporter>());
    importers.emplace_back(std::make_unique<AssImp>());
    ResourceRegistry::getDefaultRegistry().setImporters(std::move(importers));

    ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>("assets/"));

    auto tux = ResourceHandle<ImageRGBA>(Uri("file://images/tux.png"));
    auto smiley = ResourceHandle<ImageRGBA>(Uri("file://images/awesomeface.png"));
    auto font = ResourceHandle<Font>(Uri("file://fonts/Sono/static/Sono/Sono-Bold.ttf"));

    auto glfw = glfw::GLFW();
    auto runtime = std::make_shared<opengl::OpenGL>();

    const std::shared_ptr window = std::move(glfw.createWindow(runtime->getGraphicsAPI()));

    window->setWindowSize({1000, 900});

    runtime->setWindow(window);

    const auto &tuxImg = tux.get();
    const auto &smileyImg = smiley.get();

    auto freeType = std::make_unique<freetype::FreeType>();

    auto config = std::make_shared<RenderConfiguration>();

    auto scene = createScene();
    config->setScene(scene);

    auto registry = std::make_shared<SharedResourceRegistry>();

    auto passScheduler = std::make_shared<RenderPassScheduler>(runtime);

    auto graph3D = passScheduler->addGraph({
        std::make_shared<ConstructionPass>(config, registry),
        std::make_shared<CanvasRenderPass>(config, registry),
        std::make_shared<CompositingPass>(config, registry),
    });

    FrameLimiter frameLimiter(0);
    frameLimiter.reset();

    auto textLayoutEngine = TextLayoutEngine(*freeType, font, {0, 30});
    auto text = textLayoutEngine.getLayout(
        "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.",
        {
            500,
            0,
            TEXT_ALIGN_LEFT,
        });

    auto deltaTextLayoutEngine = TextLayoutEngine(*freeType, font, {0, 20});

    auto deltaText = deltaTextLayoutEngine.getLayout(std::to_string(0) + " FPS",
                                                     {
                                                         0,
                                                         0,
                                                         TEXT_ALIGN_LEFT
                                                     });
    std::chrono::milliseconds fpsUpdateInterval = std::chrono::milliseconds(50);
    auto now = std::chrono::steady_clock::now();

    RenderGraphStatistics stats;
    while (!window->shouldClose()) {
        frameLimiter.newFrame();
        window->update();

        if (std::chrono::steady_clock::now() - now > fpsUpdateInterval) {
            now = std::chrono::steady_clock::now();

            auto txt = std::to_string(frameLimiter.getFramerate()) + " FPS\n\n";
            txt += std::to_string(stats.drawCalls) + " draw calls\n";
            txt += std::to_string(stats.polygons) + " polygons\n\n";
            txt += "VRAM Usage\n";
            txt += "Total " + std::to_string(
                (stats.vertexVRamUsage + stats.indexVRamUsage + stats.shaderBufferVRamUsage + stats.textureVRamUsage) /
                MEGABYTE) + "MB\n";
            txt += "Vertex Buffers " + std::to_string(stats.vertexVRamUsage / KILOBYTE) + "KB\n";
            txt += "Index Buffers " + std::to_string(stats.indexVRamUsage / KILOBYTE) + "KB\n";
            txt += "Shader Storage Buffers " + std::to_string(stats.shaderBufferVRamUsage / KILOBYTE) + "KB\n";
            txt += "Texture Buffers " + std::to_string(stats.textureVRamUsage / KILOBYTE) + "KB\n\n";
            txt += "VRAM Copy\n";
            txt += "Vertex Buffers " + std::to_string(stats.vertexVRamCopy / KILOBYTE) + "KB\n";
            txt += "Index Buffers " + std::to_string(stats.indexVRamCopy / KILOBYTE) + "KB\n";
            txt += "Shader Storage Buffers " + std::to_string(stats.shaderBufferVRamCopy / KILOBYTE) + "KB\n";
            txt += "Texture Buffers " + std::to_string(stats.textureVRamCopy / KILOBYTE) + "KB\n\n";
            txt += "VRAM Upload\n";
            txt += "Vertex Buffers " + std::to_string(stats.vertexVRamUpload / KILOBYTE) + "KB\n";
            txt += "Index Buffers " + std::to_string(stats.indexVRamUpload / KILOBYTE) + "KB\n";
            txt += "Shader Storage Buffers " + std::to_string(stats.shaderBufferVRamUpload / KILOBYTE) + "KB\n";
            txt += "Texture Buffers " + std::to_string(stats.textureVRamUpload / KILOBYTE) + "KB\n";
            deltaText = deltaTextLayoutEngine.getLayout(txt,
                                                        {
                                                            0,
                                                            0,
                                                            TEXT_ALIGN_RIGHT
                                                        });
        }

        auto fbSize = passScheduler->updateBackBuffer();

        scene.camera.aspectRatio = static_cast<float>(fbSize.x)
                                   / static_cast<float>(fbSize.y);
        config->setScene(scene);

        auto fbSizeF = fbSize.convert<float>();

        Canvas canvas(fbSize);
        canvas.setBackgroundColor(ColorRGBA::white());
        canvas.paint(PaintLine(Vec2f(0, 0), fbSizeF, ColorRGBA::green()));
        canvas.paint(PaintLine(Vec2f(0, fbSizeF.y), Vec2f(fbSizeF.x, 0), ColorRGBA::green()));
        canvas.paint(PaintImage(Rectf({}, smileyImg.getResolution().convert<float>()),
                                Rectf({}, smileyImg.getResolution().convert<float>()),
                                smiley,
                                true));
        canvas.paint(PaintImage(Rectf({}, tuxImg.getResolution().convert<float>()),
                                Rectf({}, tuxImg.getResolution().convert<float>()),
                                tux,
                                true));
        canvas.paint(PaintLine(Vec2f(0, fbSizeF.y / 2), Vec2f(fbSizeF.x, fbSizeF.y / 2), ColorRGBA::red()));
        canvas.paint(PaintLine(Vec2f(fbSizeF.x / 2, 0), Vec2f(fbSizeF.x / 2, fbSizeF.y), ColorRGBA::red()));
        canvas.paint(PaintText({15, 10}, text, ColorRGBA::purple()));
        canvas.paint(PaintText(Vec2f(fbSizeF.x - static_cast<float>(deltaText.size.x) - 3, 0), deltaText,
                               ColorRGBA::black()));

        config->setCanvases({canvas});

        stats = passScheduler->execute(graph3D);
    }

    return 0;
}
