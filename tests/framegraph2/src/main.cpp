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

#include "xng/adapters/glfw/glfw.hpp"
#include "xng/adapters/opengl/opengl.hpp"

#include "xng/xng.hpp"

#include "shadertestpass.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::unique_ptr<ResourceImporter> > importers;
    importers.emplace_back(std::make_unique<StbiImporter>());
    ResourceRegistry::getDefaultRegistry().setImporters(std::move(importers));

    ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>("assets/"));

    auto smiley = ResourceHandle<ImageRGBA>(Uri("file://images/awesomeface.png"));

    auto glfw = glfw::GLFW();
    auto runtime = opengl::OpenGL();

    const auto window = glfw.createWindow(runtime.getGraphicsAPI());

    runtime.setWindow(*window);

    // Print shader test pass
    {
        RenderGraphBuilder builder;
        ShaderTestPass pass;
        pass.setup(builder);

        const auto graph = builder.build();
        const auto gh = runtime.compile(graph);

        runtime.execute(gh);
        runtime.destroy(gh);
    }

    RenderGraphBuilder builder = {};

    RenderPass2D pass2D;
    pass2D.setup(builder);

    auto graph = builder.build();
    auto gh = runtime.compile(graph);

    auto &ren2D = pass2D.getRenderer2D();

    const auto& smileyImg = smiley.get();

    auto tex = ren2D.createTexture(smileyImg);

    while (!window->shouldClose()) {
        window->update();

        auto fbSize = window->getFramebufferSize();

        ren2D.renderBegin(fbSize, ColorRGBA::white());
        ren2D.draw(Vec2f(0, 0), fbSize.convert<float>(), ColorRGBA::green());
        ren2D.draw(Vec2f(0, static_cast<float>(fbSize.y)),
                   Vec2f(static_cast<float>(fbSize.x), 0),
                   ColorRGBA::green());
        ren2D.draw(Rectf({}, smileyImg.getResolution().convert<float>()),
                   Rectf({}, smileyImg.getResolution().convert<float>()),
                   tex,
                   {},
                   0,
                   LINEAR);
        ren2D.renderPresent();

        if (pass2D.shouldRebuild()) {
            builder = {};
            pass2D.setup(builder);
            graph = builder.build();
            runtime.recompile(gh, graph);
        }

        runtime.execute(gh);

        window->swapBuffers();
    }

    ren2D.destroyTexture(tex);

    return 0;
}
