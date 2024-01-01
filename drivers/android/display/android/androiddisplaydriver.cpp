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

#include "xng/driver/android/androiddisplaydriver.hpp"

#include <EGL/egl.h>

#ifdef DRIVER_ANDROID_OPENGL

#include "display/android/opengl/androidwindowgles.hpp"

#endif

namespace xng::android {
    AndroidDisplayDriver::AndroidDisplayDriver() {
        app = AndroidApp::getApp();
    }

    std::unique_ptr<Window> AndroidDisplayDriver::getWindow(GpuDriverBackend gpuDriverBackend,
                                                            WindowAttributes attributes) {
        switch (gpuDriverBackend) {
#ifdef DRIVER_ANDROID_OPENGL
            case OPENGL_4_6:
                return std::make_unique<AndroidWindowGLES>(app);
#endif
            default:
                throw std::runtime_error("Unsupported gpu backend");
        }
    }
}