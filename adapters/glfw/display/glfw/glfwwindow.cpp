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

#ifdef BUILD_GLFW_OPENGL

#include <stdexcept>
#include <mutex>

#include "display/glfw/glfwwindow.hpp"

namespace xng::glfw {
    std::mutex windowMappingMutex;
    std::map<GLFWwindow *, GLFWWindow *> _windowMapping;

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
    void GLFWWindow::createWindow(const std::string &title,
                                  const Vec2i &size,
                                  const WindowAttributes &attributes) {
        applyHints(attributes);

        wndH = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
        if (wndH == nullptr) {
            throw std::runtime_error("Failed to create GLFW Window");
        }

        input = std::make_unique<GLFWInput>(*wndH);

        std::lock_guard<std::mutex> guard(windowMappingMutex);
        _windowMapping[wndH] = this;
        setCallbacks(wndH);
    }

    void GLFWWindow::createWindow(const std::string &title,
                                  const Vec2i &size,
                                  const WindowAttributes &attributes,
                                  const MonitorGLFW &monitor,
                                  const VideoMode &videoMode) {
        applyHints(attributes);

        //Setup monitor video mode hints
        glfwWindowHint(GLFW_RED_BITS, videoMode.redBits);
        glfwWindowHint(GLFW_GREEN_BITS, videoMode.greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, videoMode.blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, videoMode.refreshRate);

        wndH = glfwCreateWindow(size.x, size.y, title.c_str(), monitor.monH, nullptr);
        if (wndH == nullptr) {
            throw std::runtime_error("Failed to create GLFW Window");
        }

        input = std::make_unique<GLFWInput>(*wndH);

        std::lock_guard<std::mutex> guard(windowMappingMutex);
        _windowMapping[wndH] = this;
        setCallbacks(wndH);
    }

    GLFWWindow::~GLFWWindow() {
        std::lock_guard<std::mutex> guard(windowMappingMutex);
        _windowMapping.erase(wndH);
        glfwDestroyWindow(wndH);
    }

    void GLFWWindow::glfwWindowCloseCallback() const {
        for (auto listener: listeners) {
            listener.get().onWindowClose();
        }
    }

    void GLFWWindow::glfwWindowMoveCallback(const Vec2i &pos) const {
        for (auto listener: listeners) {
            listener.get().onWindowMove(pos);
        }
    }

    void GLFWWindow::glfwWindowSizeCallback(const int width, const int height) const {
        for (auto listener: listeners) {
            listener.get().onWindowResize(Vec2i(width, height));
        }
    }

    void GLFWWindow::glfwWindowRefreshCallback() const {
        for (auto listener: listeners) {
            listener.get().onWindowRefresh();
        }
    }

    void GLFWWindow::glfwWindowFocusCallback(const bool focused) const {
        for (auto listener: listeners) {
            listener.get().onWindowFocus(focused);
        }
    }

    void GLFWWindow::glfwWindowMinimizeCallback() const {
        for (auto listener: listeners) {
            listener.get().onWindowMinimize();
        }
    }

    void GLFWWindow::glfwWindowMaximizeCallback() const {
        for (auto listener: listeners) {
            listener.get().onWindowMaximize();
        }
    }

    void GLFWWindow::glfwWindowContentScaleCallback(const Vec2f &scale) const {
        for (auto listener: listeners) {
            listener.get().onWindowContentScale(scale);
        }
    }

    void GLFWWindow::glfwFrameBufferSizeCallback(const Vec2i &size) const {
        for (auto listener: listeners) {
            listener.get().onFramebufferResize(size);
        }
    }

    Input &GLFWWindow::getInput() {
        return *input;
    }

    void GLFWWindow::swapBuffers() {
        glfwSwapBuffers(wndH);
    }

    void GLFWWindow::update() {
        input->update();
        glfwPollEvents();
    }

    bool GLFWWindow::shouldClose() {
        return glfwWindowShouldClose(wndH);
    }

    void GLFWWindow::maximize() {
        glfwMaximizeWindow(wndH);
    }

    void GLFWWindow::minimize() {
        glfwIconifyWindow(wndH);
    }

    void GLFWWindow::restore() {
        glfwRestoreWindow(wndH);
    }

    void GLFWWindow::show() {
        glfwShowWindow(wndH);
    }

    void GLFWWindow::hide() {
        glfwHideWindow(wndH);
    }

    void GLFWWindow::focus() {
        glfwFocusWindow(wndH);
    }

    void GLFWWindow::requestAttention() {
        glfwRequestWindowAttention(wndH);
    }

    void GLFWWindow::setTitle(const std::string title) {
        glfwSetWindowTitle(wndH, title.c_str());
    }

    void GLFWWindow::setIcon(ImageRGBA &buffer) {
        throw std::runtime_error("Not Implemented");
    }

    void GLFWWindow::setWindowPosition(const Vec2i position) {
        glfwSetWindowPos(wndH, position.x, position.y);
    }

    Vec2i GLFWWindow::getWindowPosition() {
        int x, y;
        glfwGetWindowPos(wndH, &x, &y);
        return {x, y};
    }

    void GLFWWindow::setWindowSize(const Vec2i size) {
        glfwSetWindowSize(wndH, size.x, size.y);
    }

    Vec2i GLFWWindow::getWindowSize() {
        int x, y;
        glfwGetWindowSize(wndH, &x, &y);
        return {x, y};
    }

    void GLFWWindow::setWindowSizeLimit(const Vec2i sizeMin, const Vec2i sizeMax) {
        glfwSetWindowSizeLimits(wndH, sizeMin.x, sizeMin.y, sizeMax.x, sizeMax.y);
    }

    void GLFWWindow::setWindowAspectRatio(const Vec2i ratio) {
        glfwSetWindowAspectRatio(wndH, ratio.x, ratio.y);
    }

    Vec2i GLFWWindow::getFramebufferSize() {
        int x, y;
        glfwGetFramebufferSize(wndH, &x, &y);
        return {x, y};
    }

    Vec4i GLFWWindow::getFrameSize() {
        int x, y, z, w;
        glfwGetWindowFrameSize(wndH, &x, &y, &z, &w);
        return {x, y, z, w};
    }

    Vec2f GLFWWindow::getWindowContentScale() {
        float x, y;
        glfwGetWindowContentScale(wndH, &x, &y);
        return {x, y};
    }

    float GLFWWindow::getWindowOpacity() {
        float ret = glfwGetWindowOpacity(wndH);
        return ret;
    }

    void GLFWWindow::setWindowOpacity(const float opacity) {
        glfwSetWindowOpacity(wndH, opacity);
    }

    std::unique_ptr<Monitor> GLFWWindow::getMonitor() {
        return std::make_unique<MonitorGLFW>(glfwGetWindowMonitor(wndH));
    }

    void GLFWWindow::setMonitor(Monitor &monitor, const Recti rect, const int refreshRate) {
        const auto &mon = down_cast<MonitorGLFW &>(monitor);
        glfwSetWindowMonitor(wndH,
                             mon.monH,
                             rect.position.x,
                             rect.position.y,
                             rect.dimensions.x,
                             rect.dimensions.y,
                             refreshRate);
    }

    void GLFWWindow::setWindowed() {
        glfwSetWindowMonitor(wndH, nullptr, 0, 0, 0, 0, 0);
    }

    void GLFWWindow::setWindowDecorated(const bool decorated) {
        glfwSetWindowAttrib(wndH, GLFW_DECORATED, decorated);
    }

    void GLFWWindow::setWindowResizable(const bool resizable) {
        glfwSetWindowAttrib(wndH, GLFW_RESIZABLE, resizable);
    }

    void GLFWWindow::setWindowAlwaysOnTop(const bool alwaysOnTop) {
        glfwSetWindowAttrib(wndH, GLFW_FLOATING, alwaysOnTop);
    }

    void GLFWWindow::setWindowAutoMinimize(const bool autoMinimize) {
        glfwSetWindowAttrib(wndH, GLFW_AUTO_ICONIFY, autoMinimize);
    }

    void GLFWWindow::setWindowFocusOnShow(const bool focusOnShow) {
        glfwSetWindowAttrib(wndH, GLFW_FOCUS_ON_SHOW, focusOnShow);
    }

    void GLFWWindow::addListener(WindowListener &listener) {
        if (std::find_if(listeners.begin(),
                         listeners.end(),
                         [&listener](const std::reference_wrapper<WindowListener> &ref) {
                             return &ref.get() == &listener;
                         }) != listeners.end())
            throw std::runtime_error("Listener already registered");
        listeners.insert(listener);
    }

    void GLFWWindow::removeListener(WindowListener &listener) {
        listeners.erase(listener);
    }
}

#endif
