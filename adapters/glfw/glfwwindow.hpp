/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_GLFWWINDOW_HPP
#define XENGINE_GLFWWINDOW_HPP

#include <set>

#include "xng/display/window.hpp"
#include "xng/display/windowattributes.hpp"

#include "monitorglfw.hpp"
#include "glfwinput.hpp"

namespace xng::glfw {
    class GLFWWindow : public Window {
    public:
        GLFWWindow() = default;

        ~GLFWWindow() override;

        void createWindow(const std::string &title, const Vec2i& size, const WindowAttributes &attributes);

        void createWindow(const std::string &title,
                          const Vec2i& size,
                          const WindowAttributes &attributes,
                          const MonitorGLFW &monitor,
                          const VideoMode &videoMode);

        Input &getInput() override;

        void swapBuffers() override = 0;

        void update() override;

        bool shouldClose() override;

        void maximize() override;

        void minimize() override;

        void restore() override;

        void show() override;

        void hide() override;

        void focus() override;

        void requestAttention() override;

        void setTitle(std::string title) override;

        void setIcon(ImageRGBA &buffer) override;

        void setWindowPosition(Vec2i position) override;

        Vec2i getWindowPosition() override;

        void setWindowSize(Vec2i size) override;

        Vec2i getWindowSize() override;

        void setWindowSizeLimit(Vec2i sizeMin, Vec2i sizeMax) override;

        void setWindowAspectRatio(Vec2i ratio) override;

        Vec2i getFramebufferSize() override;

        Vec4i getFrameSize() override;

        Vec2f getWindowContentScale() override;

        float getWindowOpacity() override;

        void setWindowOpacity(float opacity) override;

        std::unique_ptr<Monitor> getMonitor() override;

        void setMonitor(Monitor &monitor, Recti rect, int refreshRate) override;

        void setWindowed() override;

        void setWindowDecorated(bool decorated) override;

        void setWindowResizable(bool resizable) override;

        void setWindowAlwaysOnTop(bool alwaysOnTop) override;

        void setWindowAutoMinimize(bool autoMinimize) override;

        void setWindowFocusOnShow(bool focusOnShow) override;

        void glfwWindowCloseCallback()const ;

        void glfwWindowMoveCallback(const Vec2i& pos)const ;

        void glfwWindowSizeCallback(int width, int height)const ;

        void glfwWindowRefreshCallback()const ;

        void glfwWindowFocusCallback(bool focused)const ;

        void glfwWindowMinimizeCallback()const ;

        void glfwWindowMaximizeCallback() const;

        void glfwWindowContentScaleCallback(const Vec2f& scale)const ;

        void glfwFrameBufferSizeCallback(const Vec2i& size)const ;

        void addListener(WindowListener &listener) override;

        void removeListener(WindowListener &listener) override;

        GLFWwindow *windowHandle() const { return wndH; }

    protected:
        struct CompareRefs {
            bool operator()(const std::reference_wrapper<WindowListener>& a,
                            const std::reference_wrapper<WindowListener>& b) const {
                return &a.get() < &b.get();
            }
        };

        GLFWwindow *wndH{};
        std::unique_ptr<GLFWInput> input;
        std::set<std::reference_wrapper<WindowListener>, CompareRefs> listeners;
    };
}

#endif //XENGINE_GLFWWINDOW_HPP
