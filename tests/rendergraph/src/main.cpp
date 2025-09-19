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

#include "xng/adapters/glfw/glfw.hpp"
#include "xng/adapters/opengl/opengl.hpp"
#include "xng/adapters/freetype/freetype.hpp"
#include "xng/adapters/assimp/assimp.hpp"

#include "shadertestpass.hpp"

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
    auto runtime = opengl::OpenGL();

    const std::shared_ptr window = glfw.createWindow(runtime.getGraphicsAPI());

    runtime.setWindow(window);

    // Print shader test pass
    {
        RenderGraphBuilder builder(window->getFramebufferSize());
        ShaderTestPass pass;
        pass.setup(builder);

        const auto graph = builder.build();
        const auto gh = runtime.compile(graph);

        runtime.execute(gh);
        runtime.destroy(gh);
    }

    const auto &tuxImg = tux.get();
    const auto &smileyImg = smiley.get();

    auto ren2D = Renderer2D();

    auto tuxTexture = ren2D.createTexture(tuxImg);
    auto smileyTexture = ren2D.createTexture(smileyImg);

    auto freeType = std::make_shared<freetype::FreeType>();
    auto fontRenderer = freeType->createFontRenderer(font.get());
    auto textRenderer = TextRenderer(*fontRenderer, ren2D, {0, 50});

    auto text = textRenderer.render("Hello\nWorld!", {50, 0, 0, TEXT_ALIGN_LEFT});

    auto config = std::make_shared<RenderConfiguration>();

    auto scene = createScene();
    config->setScene(scene);

    auto registry = std::make_shared<RenderRegistry>();

    auto passScheduler = std::make_shared<RenderPassScheduler>(runtime);

    auto pass2D = std::make_shared<RenderPass2D>();

    auto graph3D = passScheduler->addGraph({
        std::make_shared<ClearPass>(config, registry),
        std::make_shared<ConstructionPass>(config, registry)
    });
    auto graph2D = passScheduler->addGraph(pass2D);

    while (!window->shouldClose()) {
        window->update();

        scene.camera.aspectRatio = static_cast<float>(window->getFramebufferSize().x)
                                / static_cast<float>(window->getFramebufferSize().y);
        config->setScene(scene);

        auto fbSize = window->getFramebufferSize();
        auto fbSizeF = fbSize.convert<float>();

        ren2D.renderBegin(fbSize, ColorRGBA::white());

        ren2D.draw(Vec2f(0, 0), fbSizeF, ColorRGBA::green());
        ren2D.draw(Vec2f(0, fbSizeF.y), Vec2f(fbSizeF.x, 0), ColorRGBA::green());
        ren2D.draw({}, smileyTexture, LINEAR);
        ren2D.draw({}, tuxTexture, LINEAR);
        ren2D.draw(Vec2f(0, fbSizeF.y / 2), Vec2f(fbSizeF.x, fbSizeF.y / 2), ColorRGBA::red());
        ren2D.draw(Vec2f(fbSizeF.x / 2, 0), Vec2f(fbSizeF.x / 2, fbSizeF.y), ColorRGBA::red());
        ren2D.draw(Vec2f(50, 0), text, ColorRGBA::purple());

        ren2D.renderPresent();

        pass2D->setBatches(ren2D.getRenderBatches());
        ren2D.clearBatches();

        passScheduler->execute({graph3D, graph2D});

        window->swapBuffers();
    }

    ren2D.destroyTexture(smileyTexture);

    return 0;
}
