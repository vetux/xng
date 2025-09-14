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

#include "xng/render/2d/renderpass2d.hpp"

#include "shadertestpass.hpp"

int main(int argc, char *argv[]) {
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

    while (!window->shouldClose()) {
        window->update();

        auto fbSize = window->getFramebufferSize();

        ren2D.renderBegin(fbSize, ColorRGBA::olive());
        ren2D.draw(Vec2f(0, 0), fbSize.convert<float>(), ColorRGBA::green());
        ren2D.renderPresent();

        if (pass2D.shouldRebuild()) {
            builder = {};
            pass2D.setup(builder);
            graph = builder.build();
            runtime.recompile(gh, graph);
        }

        runtime.execute(gh);
    }

    return 0;
}
