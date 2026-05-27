/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

    class SurfaceGL final : public rg::Surface {
    public:
        explicit SurfaceGL(std::shared_ptr<Window> _window)
            : window(std::move(_window)) {
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
