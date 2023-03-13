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

#include "xng/android/androidapp.hpp"

android_app *xng::android::AndroidApp::app = nullptr;

namespace xng {
    namespace android {
        android_app *AndroidApp::getApp() {
            if (app) {
                return app;
            } else {
                throw std::runtime_error("No app assigned.");
            }
        }

        void AndroidApp::setApp(android_app *appArg) {
            app = appArg;
        }
    }
}
