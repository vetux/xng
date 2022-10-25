/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include <stdexcept>
#include <mutex>

#include "windowglfwgl.hpp"

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

namespace xng {
    namespace glfw {
        std::mutex windowMappingMutex;
        std::map<GLFWwindow *, WindowGLFWGL *> _windowMapping;

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
            glfwWindowHint(GLFW_SAMPLES, attribs.samples);
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
        WindowGLFWGL::WindowGLFWGL(const std::string &title, Vec2i size, WindowAttributes attributes) {
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

            renderTargetGl = std::make_unique<GLFWRenderTargetGL>(*wndH);

            input = std::make_unique<GLFWInput>(*wndH);

            std::lock_guard<std::mutex> guard(windowMappingMutex);
            _windowMapping[wndH] = this;
            setCallbacks(wndH);
        }

        WindowGLFWGL::WindowGLFWGL(const std::string &title,
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

            renderTargetGl = std::make_unique<GLFWRenderTargetGL>(*wndH);

            input = std::make_unique<GLFWInput>(*wndH);

            std::lock_guard<std::mutex> guard(windowMappingMutex);
            _windowMapping[wndH] = this;
            setCallbacks(wndH);
        }

        WindowGLFWGL::~WindowGLFWGL() {
            std::lock_guard<std::mutex> guard(windowMappingMutex);
            _windowMapping.erase(wndH);
            glfwDestroyWindow(wndH);
            GLFWCounter::leave();
        }

        void WindowGLFWGL::glfwWindowCloseCallback() {
            for (auto listener: listeners) {
                listener->onWindowClose();
            }
        }

        void WindowGLFWGL::glfwWindowMoveCallback(Vec2i pos) {
            for (auto listener: listeners) {
                listener->onWindowMove(pos);
            }
        }

        void WindowGLFWGL::glfwWindowSizeCallback(int width, int height) {
            renderTargetGl->desc.size = {width, height};
            for (auto listener: listeners) {
                listener->onWindowResize(Vec2i(width, height));
            }
        }

        void WindowGLFWGL::glfwWindowRefreshCallback() {
            for (auto listener: listeners) {
                listener->onWindowRefresh();
            }
        }

        void WindowGLFWGL::glfwWindowFocusCallback(bool focused) {
            for (auto listener: listeners) {
                listener->onWindowFocus(focused);
            }
        }

        void WindowGLFWGL::glfwWindowMinimizeCallback() {
            for (auto listener: listeners) {
                listener->onWindowMinimize();
            }
        }

        void WindowGLFWGL::glfwWindowMaximizeCallback() {
            for (auto listener: listeners) {
                listener->onWindowMaximize();
            }
        }

        void WindowGLFWGL::glfwWindowContentScaleCallback(Vec2f scale) {
            for (auto listener: listeners) {
                listener->onWindowContentScale(scale);
            }
        }

        void WindowGLFWGL::glfwFrameBufferSizeCallback(Vec2i size) {
            for (auto listener: listeners) {
                listener->onFramebufferResize(size);
            }
        }

        RenderTarget &WindowGLFWGL::getRenderTarget() {
            return *renderTargetGl;
        }

        Input &WindowGLFWGL::getInput() {
            return dynamic_cast<Input &>(*input);
        }

        std::string WindowGLFWGL::getDisplayDriver() {
            return "glfw";
        }

        std::string WindowGLFWGL::getGraphicsDriver() {
            return "opengl";
        }

        void WindowGLFWGL::bindGraphics() {
            glfwMakeContextCurrent(wndH);
        }

        void WindowGLFWGL::unbindGraphics() {
            glfwMakeContextCurrent(nullptr);
        }

        void WindowGLFWGL::swapBuffers() {
            glfwSwapBuffers(wndH);
        }

        void WindowGLFWGL::update() {
            input->update();
            glfwPollEvents();
        }

        bool WindowGLFWGL::shouldClose() {
            return glfwWindowShouldClose(wndH);
        }

        void WindowGLFWGL::maximize() {
            glfwMaximizeWindow(wndH);
        }

        void WindowGLFWGL::minimize() {
            glfwIconifyWindow(wndH);
        }

        void WindowGLFWGL::restore() {
            glfwRestoreWindow(wndH);
        }

        void WindowGLFWGL::show() {
            glfwShowWindow(wndH);
        }

        void WindowGLFWGL::hide() {
            glfwHideWindow(wndH);
        }

        void WindowGLFWGL::focus() {
            glfwFocusWindow(wndH);
        }

        void WindowGLFWGL::requestAttention() {
            glfwRequestWindowAttention(wndH);
        }

        void WindowGLFWGL::setTitle(std::string title) {
            glfwSetWindowTitle(wndH, title.c_str());
        }

        void WindowGLFWGL::setIcon(ImageRGBA &buffer) {
            throw std::runtime_error("Not Implemented");
        }

        void WindowGLFWGL::setWindowPosition(Vec2i position) {
            glfwSetWindowPos(wndH, position.x, position.y);
        }

        Vec2i WindowGLFWGL::getWindowPosition() {
            int x, y;
            glfwGetWindowPos(wndH, &x, &y);
            return {x, y};
        }

        void WindowGLFWGL::setWindowSize(Vec2i size) {
            glfwSetWindowSize(wndH, size.x, size.y);
        }

        Vec2i WindowGLFWGL::getWindowSize() {
            int x, y;
            glfwGetWindowSize(wndH, &x, &y);
            return {x, y};
        }

        void WindowGLFWGL::setWindowSizeLimit(Vec2i sizeMin, Vec2i sizeMax) {
            glfwSetWindowSizeLimits(wndH, sizeMin.x, sizeMin.y, sizeMax.x, sizeMax.y);
        }

        void WindowGLFWGL::setWindowAspectRatio(Vec2i ratio) {
            glfwSetWindowAspectRatio(wndH, ratio.x, ratio.y);
        }

        Vec2i WindowGLFWGL::getFramebufferSize() {
            int x, y;
            glfwGetFramebufferSize(wndH, &x, &y);
            return {x, y};
        }

        Vec4i WindowGLFWGL::getFrameSize() {
            int x, y, z, w;
            glfwGetWindowFrameSize(wndH, &x, &y, &z, &w);
            return {x, y, z, w};
        }

        Vec2f WindowGLFWGL::getWindowContentScale() {
            float x, y;
            glfwGetWindowContentScale(wndH, &x, &y);
            return {x, y};
        }

        float WindowGLFWGL::getWindowOpacity() {
            float ret = glfwGetWindowOpacity(wndH);
            return ret;
        }

        void WindowGLFWGL::setWindowOpacity(float opacity) {
            glfwSetWindowOpacity(wndH, opacity);
        }

        std::unique_ptr<Monitor> WindowGLFWGL::getMonitor() {
            return std::make_unique<MonitorGLFW>(glfwGetWindowMonitor(wndH));
        }

        void WindowGLFWGL::setMonitor(Monitor &monitor, Recti rect, int refreshRate) {
            auto &mon = dynamic_cast<MonitorGLFW &>(monitor);
            glfwSetWindowMonitor(wndH,
                                 mon.monH,
                                 rect.position.x,
                                 rect.position.y,
                                 rect.dimensions.x,
                                 rect.dimensions.y,
                                 refreshRate);
        }

        void WindowGLFWGL::setWindowed() {
            glfwSetWindowMonitor(wndH, nullptr, 0, 0, 0, 0, 0);
        }

        void WindowGLFWGL::setWindowDecorated(bool decorated) {
            glfwSetWindowAttrib(wndH, GLFW_DECORATED, decorated);
        }

        void WindowGLFWGL::setWindowResizable(bool resizable) {
            glfwSetWindowAttrib(wndH, GLFW_RESIZABLE, resizable);
        }

        void WindowGLFWGL::setWindowAlwaysOnTop(bool alwaysOnTop) {
            glfwSetWindowAttrib(wndH, GLFW_FLOATING, alwaysOnTop);
        }

        void WindowGLFWGL::setWindowAutoMinimize(bool autoMinimize) {
            glfwSetWindowAttrib(wndH, GLFW_AUTO_ICONIFY, autoMinimize);
        }

        void WindowGLFWGL::setWindowFocusOnShow(bool focusOnShow) {
            glfwSetWindowAttrib(wndH, GLFW_FOCUS_ON_SHOW, focusOnShow);
        }

        void WindowGLFWGL::setSwapInterval(int interval) {
            bindGraphics();
            glfwSwapInterval(interval);
        }

        Listenable<WindowListener>::UnregisterCallback WindowGLFWGL::addListener(WindowListener &listener) {
            if (listeners.find(&listener) != listeners.end())
                throw std::runtime_error("Listener already registered");
            listeners.insert(&listener);
            return [this, &listener]() {
                removeListener(listener);
            };
        }

        void WindowGLFWGL::removeListener(WindowListener &listener) {
            listeners.erase(&listener);
        }
    }
}