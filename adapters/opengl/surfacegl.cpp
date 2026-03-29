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

#include "surfacegl.hpp"

namespace xng::opengl {
    void SurfaceGL::update() {
            oglDebugStartGroup("Surface::update");

            if (backBuffer == nullptr) {
                backBuffer = std::make_shared<Framebuffer>();
            }

            auto fbSize = window->getFramebufferSize();

            // Handle framebuffer null size (Minimize, Window resize)
            if (fbSize.x <= 0) fbSize.x = 1;
            if (fbSize.y <= 0) fbSize.y = 1;

            if (backBufferColor == nullptr || backBufferColor->desc.size != fbSize) {
                rendergraph::Texture desc;
                desc.size = fbSize;
                desc.format = RGBA;
                backBufferColor = std::make_shared<TextureGL>(desc);

                glBindFramebuffer(GL_FRAMEBUFFER, backBuffer->FBO);
                {
                    Framebuffer::attach2D(GL_COLOR_ATTACHMENT0, *backBufferColor, backBufferColor->textureType, 0);

                    glDrawBuffer(GL_COLOR_ATTACHMENT0);

                    const auto clearColor = ColorRGBA::fuchsia();
                    glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), 0);
                    glClearDepth(0);
                    glClearStencil(0);
                    glClear(GL_COLOR_BUFFER_BIT);
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                oglCheckError();
            }

            oglDebugEndGroup();
        }

        void SurfaceGL::present() const {
            oglDebugStartGroup("Surface::present");

            auto srcSize = backBufferColor->desc.size;
            auto dstSize = window->getFramebufferSize();

            // Skip presenting frames on resize, prevents the contents of the window from jittering when live resizing.
            if (dstSize == srcSize) {
                glBindFramebuffer(GL_READ_FRAMEBUFFER, backBuffer->FBO);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

                glClearColor(0, 0, 0, 0);
                glClear(GL_COLOR_BUFFER_BIT);

                glBlitFramebuffer(0,
                                  0,
                                  srcSize.x,
                                  srcSize.y,
                                  0,
                                  0,
                                  dstSize.x,
                                  dstSize.y,
                                  GL_COLOR_BUFFER_BIT,
                                  GL_LINEAR);

                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            }

            glFlush();

            windowGl->swapBuffers();

            oglCheckError();
            try {
                oglCheckError();
            } catch (const std::exception &e) {
                // Because the framebuffer size can change randomly, I ignore errors that glBlitFramebuffer might throw.
            }

            oglDebugEndGroup();
        }
}