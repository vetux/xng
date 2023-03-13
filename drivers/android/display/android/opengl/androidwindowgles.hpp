/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_ANDROIDWINDOW_HPP
#define XENGINE_ANDROIDWINDOW_HPP

#include "xng/display/window.hpp"

#include "androidrendertargetgles.hpp"

#include "input/android/androidinput.hpp"

namespace xng {
    namespace android {
        class AndroidWindowGLES : public Window {
        public:
            AndroidWindowGLES(android_app *app)
                    : app(app) {

            }

            RenderTarget &getRenderTarget() override {
                return target;
            }

            Input &getInput() override {
                return input;
            }

            std::string getDisplayDriver() override {
                return DisplayDriver::getBackendName(DisplayDriverBackend::DISPLAY_ANDROID);
            }

            std::string getGraphicsDriver() override {
                return std::string();
            }

            void bindGraphics() override {

            }

            void unbindGraphics() override {

            }

            void swapBuffers() override {

            }

            void update() override {

            }

            bool shouldClose() override {
                return false;
            }

            void maximize() override {

            }

            void minimize() override {

            }

            void restore() override {

            }

            void show() override {

            }

            void hide() override {

            }

            void focus() override {

            }

            void requestAttention() override {

            }

            void setTitle(std::string title) override {

            }

            void setIcon(ImageRGBA &buffer) override {

            }

            void setWindowPosition(Vec2i position) override {

            }

            Vec2i getWindowPosition() override {
                return xng::Vec2i();
            }

            void setWindowSize(Vec2i size) override {

            }

            Vec2i getWindowSize() override {
                return xng::Vec2i();
            }

            void setWindowSizeLimit(Vec2i sizeMin, Vec2i sizeMax) override {

            }

            void setWindowAspectRatio(Vec2i ratio) override {

            }

            Vec2i getFramebufferSize() override {
                return xng::Vec2i();
            }

            Vec4i getFrameSize() override {
                return xng::Vec4i();
            }

            Vec2f getWindowContentScale() override {
                return xng::Vec2f();
            }

            float getWindowOpacity() override {
                return 0;
            }

            void setWindowOpacity(float opacity) override {

            }

            std::unique_ptr<Monitor> getMonitor() override {
                return std::unique_ptr<Monitor>();
            }

            void setMonitor(Monitor &monitor, Recti rect, int refreshRate) override {

            }

            void setWindowed() override {

            }

            void setWindowDecorated(bool decorated) override {

            }

            void setWindowResizable(bool resizable) override {

            }

            void setWindowAlwaysOnTop(bool alwaysOnTop) override {

            }

            void setWindowAutoMinimize(bool autoMinimize) override {

            }

            void setWindowFocusOnShow(bool focusOnShow) override {

            }

            void setSwapInterval(int interval) override {

            }

            UnregisterCallback addListener(WindowListener &listener) override {
                return xng::Listenable<WindowListener>::UnregisterCallback();
            }

            void removeListener(WindowListener &listener) override {

            }

            android_app *app;

            AndroidRenderTargetGLES target;
            AndroidInput input;
        };
    }
}
#endif //XENGINE_ANDROIDWINDOW_HPP
