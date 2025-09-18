/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_WINDOW_HPP
#define XENGINE_WINDOW_HPP

#include "xng/render/scene/image.hpp"

#include "xng/display/windowlistener.hpp"
#include "xng/display/monitor.hpp"

#include "xng/input/input.hpp"
#include "xng/math/vector2.hpp"

namespace xng {
    class XENGINE_EXPORT Window {
    public:
        virtual ~Window() = default;

        virtual void addListener(WindowListener &listener) = 0;

        virtual void removeListener(WindowListener &listener) = 0;

        virtual Input &getInput() = 0;

        virtual void swapBuffers() = 0;

        virtual void update() = 0;

        virtual bool shouldClose() = 0;

        virtual void maximize() = 0;

        virtual void minimize() = 0;

        virtual void restore() = 0;

        virtual void show() = 0;

        virtual void hide() = 0;

        virtual void focus() = 0;

        virtual void requestAttention() = 0;

        virtual void setTitle(std::string title) = 0;

        virtual void setIcon(ImageRGBA &buffer) = 0;

        virtual void setWindowPosition(Vec2i position) = 0;

        virtual Vec2i getWindowPosition() = 0;

        virtual void setWindowSize(Vec2i size) = 0;

        virtual Vec2i getWindowSize() = 0;

        virtual void setWindowSizeLimit(Vec2i sizeMin, Vec2i sizeMax) = 0;

        virtual void setWindowAspectRatio(Vec2i ratio) = 0;

        virtual Vec2i getFramebufferSize() = 0;

        virtual Vec4i getFrameSize() = 0;

        virtual Vec2f getWindowContentScale() = 0;

        virtual float getWindowOpacity() = 0;

        virtual void setWindowOpacity(float opacity) = 0;

        virtual std::unique_ptr<Monitor> getMonitor() = 0;

        virtual void setMonitor(Monitor &monitor, Recti rect, int refreshRate) = 0;

        virtual void setWindowed() = 0;

        virtual void setWindowDecorated(bool decorated) = 0;

        virtual void setWindowResizable(bool resizable) = 0;

        virtual void setWindowAlwaysOnTop(bool alwaysOnTop) = 0;

        virtual void setWindowAutoMinimize(bool autoMinimize) = 0;

        virtual void setWindowFocusOnShow(bool focusOnShow) = 0;
    };
}

#endif //XENGINE_WINDOW_HPP
