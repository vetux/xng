/**
 *  Mana - 3D Game Engine
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef BUILD_ENGINE_DISPLAY_GLFW

#include <stdexcept>
#include <mutex>

#include "windowglfw.hpp"

namespace GLFWCounter {
    std::mutex counterMutex;
    int counter = 0;

    void join() {
        std::lock_guard<std::mutex> guard(counterMutex);
        if (counter == 0)
            glfwInit();
        counter++;
    }

    void leave() {
        std::lock_guard<std::mutex> guard(counterMutex);
        counter--;
        if (counter == 0)
            glfwTerminate();
    }
}

namespace xengine {
    namespace glfw {
        std::mutex windowMappingMutex;
        std::map<GLFWwindow *, WindowGLFW *> _windowMapping;

        void glfwWindowCloseCallback(GLFWwindow *window) {
            std::lock_guard<std::mutex> guard(windowMappingMutex);
            if (_windowMapping.find(window) != _windowMapping.end()) {
                _windowMapping[window]->glfwWindowCloseCallback();
            } else {
                fprintf(stderr, "Received glfw resize with non registered window.");
            }
        }

        void glfwWindowPositionCallback(GLFWwindow *window, int posx, int posy) {
            std::lock_guard<std::mutex> guard(windowMappingMutex);
            if (_windowMapping.find(window) != _windowMapping.end()) {
                _windowMapping[window]->glfwWindowMoveCallback(Vec2i(posx, posy));
            } else {
                fprintf(stderr, "Received glfw resize with non registered window.");
            }
        }

        void glfwWindowSizeCallback(GLFWwindow *window, int width, int height) {
            std::lock_guard<std::mutex> guard(windowMappingMutex);
            if (_windowMapping.find(window) != _windowMapping.end()) {
                _windowMapping[window]->glfwWindowSizeCallback(width, height);
            } else {
                fprintf(stderr, "Received glfw resize with non registered window.");
            }
        }

        void glfwWindowRefreshCallback(GLFWwindow *window) {
            std::lock_guard<std::mutex> guard(windowMappingMutex);
            if (_windowMapping.find(window) != _windowMapping.end()) {
                _windowMapping[window]->glfwWindowRefreshCallback();
            } else {
                fprintf(stderr, "Received glfw refresh with non registered window.");
            }
        }

        void glfwWindowFocusCallback(GLFWwindow *window, int focus) {
            std::lock_guard<std::mutex> guard(windowMappingMutex);
            if (_windowMapping.find(window) != _windowMapping.end()) {
                _windowMapping[window]->glfwWindowFocusCallback(focus == GLFW_TRUE);
            } else {
                fprintf(stderr, "Received glfw resizeCallback with non registered window.");
            }
        }

        void glfwWindowMinimizeCallback(GLFWwindow *window, int minimized) {
            std::lock_guard<std::mutex> guard(windowMappingMutex);
            if (_windowMapping.find(window) != _windowMapping.end()) {
                _windowMapping[window]->glfwWindowMinimizeCallback();
            } else {
                fprintf(stderr, "Received glfw resizeCallback with non registered window.");
            }
        }

        void glfwWindowMaximizeCallback(GLFWwindow *window, int maximized) {
            std::lock_guard<std::mutex> guard(windowMappingMutex);
            if (_windowMapping.find(window) != _windowMapping.end()) {
                _windowMapping[window]->glfwWindowMaximizeCallback();
            } else {
                fprintf(stderr, "Received glfw resizeCallback with non registered window.");
            }
        }

        void glfwWindowContentScaleCallback(GLFWwindow *window, float scaleX, float scaleY) {
            std::lock_guard<std::mutex> guard(windowMappingMutex);
            if (_windowMapping.find(window) != _windowMapping.end()) {
                _windowMapping[window]->glfwWindowContentScaleCallback(Vec2f(scaleX, scaleY));
            } else {
                fprintf(stderr, "Received glfw resizeCallback with non registered window.");
            }
        }

        void glfwFrameBufferSizeCallback(GLFWwindow *window, int sizeX, int sizeY) {
            std::lock_guard<std::mutex> guard(windowMappingMutex);
            if (_windowMapping.find(window) != _windowMapping.end()) {
                _windowMapping[window]->glfwFrameBufferSizeCallback(Vec2i(sizeX, sizeY));
            } else {
                fprintf(stderr, "Received glfw resizeCallback with non registered window.");
            }
        }

        void applyHints(WindowAttributes attribs) {
            glfwWindowHint(GLFW_RESIZABLE, attribs.resizable);
            glfwWindowHint(GLFW_VISIBLE, attribs.visible);
            glfwWindowHint(GLFW_DECORATED, attribs.decorated);
            glfwWindowHint(GLFW_FOCUSED, attribs.focused);
            glfwWindowHint(GLFW_AUTO_ICONIFY, attribs.autoMinimize);
            glfwWindowHint(GLFW_FLOATING, attribs.alwaysOnTop);
            glfwWindowHint(GLFW_MAXIMIZED, attribs.maximized);
            glfwWindowHint(GLFW_CENTER_CURSOR, attribs.centerCursor);
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, attribs.transparentFramebuffer);
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, attribs.focusOnShow);
            glfwWindowHint(GLFW_SCALE_TO_MONITOR, attribs.scaleToMonitor);
            glfwWindowHint(GLFW_RED_BITS, attribs.bitsRed);
            glfwWindowHint(GLFW_GREEN_BITS, attribs.bitsGreen);
            glfwWindowHint(GLFW_BLUE_BITS, attribs.bitsBlue);
            glfwWindowHint(GLFW_ALPHA_BITS, attribs.bitsAlpha);
            glfwWindowHint(GLFW_DEPTH_BITS, attribs.bitsDepth);
            glfwWindowHint(GLFW_STENCIL_BITS, attribs.bitsStencil);
            glfwWindowHint(GLFW_SAMPLES, attribs.msaaSamples);
            glfwWindowHint(GLFW_SRGB_CAPABLE, attribs.sRGBCapable);
            glfwWindowHint(GLFW_DOUBLEBUFFER, attribs.doubleBuffer);
        }

        void setCallbacks(GLFWwindow *wndH) {
            glfwSetWindowCloseCallback(wndH, glfwWindowCloseCallback);
            glfwSetWindowPosCallback(wndH, glfwWindowPositionCallback);
            glfwSetWindowSizeCallback(wndH, glfwWindowSizeCallback);
            glfwSetWindowRefreshCallback(wndH, glfwWindowRefreshCallback);
            glfwSetWindowFocusCallback(wndH, glfwWindowFocusCallback);
            glfwSetWindowIconifyCallback(wndH, glfwWindowMinimizeCallback);
            glfwSetWindowMaximizeCallback(wndH, glfwWindowMaximizeCallback);
            glfwSetWindowContentScaleCallback(wndH, glfwWindowContentScaleCallback);
            glfwSetFramebufferSizeCallback(wndH, glfwFrameBufferSizeCallback);
        }

        //TODO: GLFW Window implementation synchronization
        WindowGLFW::WindowGLFW(const std::string &title, Vec2i size, WindowAttributes attributes) {
            GLFWCounter::join();

            glfwDefaultWindowHints();

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            applyHints(attributes);

            wndH = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
            if (wndH == nullptr) {
                GLFWCounter::leave();
                throw std::runtime_error("Failed to create GLFW Window");
            }

            glfwMakeContextCurrent(wndH);
            if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                GLFWCounter::leave();
                throw std::runtime_error("Failed to initialize glad");
            }

            glfwSwapInterval(attributes.swapInterval);

            renderTargetGl = std::make_unique<GLFWRenderTargetGL>();

            input = std::make_unique<GLFWInput>(*wndH);

            std::lock_guard<std::mutex> guard(windowMappingMutex);
            _windowMapping[wndH] = this;
            setCallbacks(wndH);
        }

        WindowGLFW::WindowGLFW(const std::string &title,
                               Vec2i size,
                               WindowAttributes attributes,
                               MonitorGLFW &monitor,
                               VideoMode videoMode) {
            GLFWCounter::join();

            glfwDefaultWindowHints();

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            applyHints(attributes);

            //Setup monitor video mode hints
            glfwWindowHint(GLFW_RED_BITS, videoMode.redBits);
            glfwWindowHint(GLFW_GREEN_BITS, videoMode.greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, videoMode.blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, videoMode.refreshRate);

            wndH = glfwCreateWindow(size.x, size.y, title.c_str(), monitor.monH, nullptr);
            if (wndH == nullptr) {
                GLFWCounter::leave();
                throw std::runtime_error("Failed to create GLFW Window");
            }

            glfwMakeContextCurrent(wndH);
            if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                GLFWCounter::leave();
                throw std::runtime_error("Failed to initialize glad");
            }


            renderTargetGl = std::make_unique<GLFWRenderTargetGL>();

            input = std::make_unique<GLFWInput>(*wndH);

            std::lock_guard<std::mutex> guard(windowMappingMutex);
            _windowMapping[wndH] = this;
            setCallbacks(wndH);
        }

        WindowGLFW::~WindowGLFW() {
            std::lock_guard<std::mutex> guard(windowMappingMutex);
            _windowMapping.erase(wndH);
            glfwDestroyWindow(wndH);
            GLFWCounter::leave();
        }

        void WindowGLFW::glfwWindowCloseCallback() {
            for (auto listener: listeners) {
                listener->onWindowClose();
            }
        }

        void WindowGLFW::glfwWindowMoveCallback(Vec2i pos) {
            for (auto listener: listeners) {
                listener->onWindowMove(pos);
            }
        }

        void WindowGLFW::glfwWindowSizeCallback(int width, int height) {
            renderTargetGl->size = {width, height};
            for (auto listener: listeners) {
                listener->onWindowResize(Vec2i(width, height));
            }
        }

        void WindowGLFW::glfwWindowRefreshCallback() {
            for (auto listener: listeners) {
                listener->onWindowRefresh();
            }
        }

        void WindowGLFW::glfwWindowFocusCallback(bool focused) {
            for (auto listener: listeners) {
                listener->onWindowFocus(focused);
            }
        }

        void WindowGLFW::glfwWindowMinimizeCallback() {
            for (auto listener: listeners) {
                listener->onWindowMinimize();
            }
        }

        void WindowGLFW::glfwWindowMaximizeCallback() {
            for (auto listener: listeners) {
                listener->onWindowMaximize();
            }
        }

        void WindowGLFW::glfwWindowContentScaleCallback(Vec2f scale) {
            for (auto listener: listeners) {
                listener->onWindowContentScale(scale);
            }
        }

        void WindowGLFW::glfwFrameBufferSizeCallback(Vec2i size) {
            for (auto listener: listeners) {
                listener->onFramebufferResize(size);
            }
        }

        RenderTarget &WindowGLFW::getRenderTarget(GraphicsBackend backend) {
            switch (backend) {
                case OPENGL_4_6:
                    return *renderTargetGl;
                default:
                    throw std::runtime_error("Unsupported graphics backend");
            }
        }

        Input &WindowGLFW::getInput() {
            return dynamic_cast<Input &>(*input);
        }

        DisplayBackend WindowGLFW::getDisplayBackend() {
            return GLFW;
        }

        void WindowGLFW::makeCurrent() {
            glfwMakeContextCurrent(wndH);
        }

        void WindowGLFW::swapBuffers() {
            glfwSwapBuffers(wndH);
        }

        void WindowGLFW::update() {
            glfwPollEvents();
        }

        bool WindowGLFW::shouldClose() {
            return glfwWindowShouldClose(wndH);
        }

        void WindowGLFW::registerListener(WindowListener &listener) {
            if (listeners.find(&listener) != listeners.end())
                throw std::runtime_error("Listener already registered");
            listeners.insert(&listener);
        }

        void WindowGLFW::unregisterListener(WindowListener &listener) {
            listeners.erase(&listener);
        }

        void WindowGLFW::maximize() {
            glfwMaximizeWindow(wndH);
        }

        void WindowGLFW::minimize() {
            glfwIconifyWindow(wndH);
        }

        void WindowGLFW::restore() {
            glfwRestoreWindow(wndH);
        }

        void WindowGLFW::show() {
            glfwShowWindow(wndH);
        }

        void WindowGLFW::hide() {
            glfwHideWindow(wndH);
        }

        void WindowGLFW::focus() {
            glfwFocusWindow(wndH);
        }

        void WindowGLFW::requestAttention() {
            glfwRequestWindowAttention(wndH);
        }

        void WindowGLFW::setTitle(std::string title) {
            glfwSetWindowTitle(wndH, title.c_str());
        }

        void WindowGLFW::setIcon(Image<ColorRGBA> &buffer) {
            throw std::runtime_error("Not Implemented");
        }

        void WindowGLFW::setWindowPosition(Vec2i position) {
            glfwSetWindowPos(wndH, position.x, position.y);
        }

        Vec2i WindowGLFW::getWindowPosition() {
            int x, y;
            glfwGetWindowPos(wndH, &x, &y);
            return {x, y};
        }

        void WindowGLFW::setWindowSize(Vec2i size) {
            glfwSetWindowSize(wndH, size.x, size.y);
        }

        Vec2i WindowGLFW::getWindowSize() {
            int x, y;
            glfwGetWindowSize(wndH, &x, &y);
            return {x, y};
        }

        void WindowGLFW::setWindowSizeLimit(Vec2i sizeMin, Vec2i sizeMax) {
            glfwSetWindowSizeLimits(wndH, sizeMin.x, sizeMin.y, sizeMax.x, sizeMax.y);
        }

        void WindowGLFW::setWindowAspectRatio(Vec2i ratio) {
            glfwSetWindowAspectRatio(wndH, ratio.x, ratio.y);
        }

        Vec2i WindowGLFW::getFramebufferSize() {
            int x, y;
            glfwGetFramebufferSize(wndH, &x, &y);
            return {x, y};
        }

        Vec4i WindowGLFW::getFrameSize() {
            int x, y, z, w;
            glfwGetWindowFrameSize(wndH, &x, &y, &z, &w);
            return {x, y, z, w};
        }

        Vec2f WindowGLFW::getWindowContentScale() {
            float x, y;
            glfwGetWindowContentScale(wndH, &x, &y);
            return {x, y};
        }

        float WindowGLFW::getWindowOpacity() {
            float ret = glfwGetWindowOpacity(wndH);
            return ret;
        }

        void WindowGLFW::setWindowOpacity(float opacity) {
            glfwSetWindowOpacity(wndH, opacity);
        }

        std::unique_ptr<Monitor> WindowGLFW::getMonitor() {
            return std::make_unique<MonitorGLFW>(glfwGetWindowMonitor(wndH));
        }

        void WindowGLFW::setMonitor(Monitor &monitor, Recti rect, int refreshRate) {
            auto &mon = dynamic_cast<MonitorGLFW &>(monitor);
            glfwSetWindowMonitor(wndH,
                                 mon.monH,
                                 rect.position.x,
                                 rect.position.y,
                                 rect.dimensions.x,
                                 rect.dimensions.y,
                                 refreshRate);
        }

        void WindowGLFW::setWindowed() {
            glfwSetWindowMonitor(wndH, nullptr, 0, 0, 0, 0, 0);
        }

        void WindowGLFW::setWindowDecorated(bool decorated) {
            glfwSetWindowAttrib(wndH, GLFW_DECORATED, decorated);
        }

        void WindowGLFW::setWindowResizable(bool resizable) {
            glfwSetWindowAttrib(wndH, GLFW_RESIZABLE, resizable);
        }

        void WindowGLFW::setWindowAlwaysOnTop(bool alwaysOnTop) {
            glfwSetWindowAttrib(wndH, GLFW_FLOATING, alwaysOnTop);
        }

        void WindowGLFW::setWindowAutoMinimize(bool autoMinimize) {
            glfwSetWindowAttrib(wndH, GLFW_AUTO_ICONIFY, autoMinimize);
        }

        void WindowGLFW::setWindowFocusOnShow(bool focusOnShow) {
            glfwSetWindowAttrib(wndH, GLFW_FOCUS_ON_SHOW, focusOnShow);
        }

        void WindowGLFW::setSwapInterval(int interval) {
            makeCurrent();
            glfwSwapInterval(interval);
        }
    }
}

#endif