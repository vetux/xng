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

#ifndef XENGINE_SURFACEGL_HPP
#define XENGINE_SURFACEGL_HPP

#include <memory>

#include "display/windowgl.hpp"
#include "xng/rendergraph/surface.hpp"

#include "resource/texturegl.hpp"
#include "xng/display/window.hpp"

namespace xng::opengl {
    class Framebuffer;

    class SurfaceGL final : public rendergraph::Surface {
    public:
        explicit SurfaceGL(std::shared_ptr<Window> window)
            : window(std::move(window)) {
            windowGl = down_cast<WindowGl *>(window.get());
            windowGl->bindContext();
            update();
            windowGl->unbindContext();
        }

        ~SurfaceGL() override = default;

        Vec2i getDimensions() override {
            return backBufferColor->desc.size;
        }

        void bindContext() const {
            windowGl->bindContext();
        }

        void unbindContext() const {
            windowGl->unbindContext();
        }

        /**
         * FBO + Texture are created on the surface context.
         *
         * The Texture is then bound in an FBO owned by the global context.
         */
        void update();

        void present() const;

        std::shared_ptr<Window> window = nullptr;
        WindowGl *windowGl = nullptr;

        std::shared_ptr<TextureGL> backBufferColor;
        std::shared_ptr<Framebuffer> backBuffer;
    };
}

#endif //XENGINE_SURFACEGL_HPP
