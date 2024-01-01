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

#ifndef XENGINE_ANDROIDAPP_HPP
#define XENGINE_ANDROIDAPP_HPP

#include <stdexcept>

struct android_app;

namespace xng {
    namespace android {
        /**
         * The android_app object which must be assigned by the user by including this header and calling setApp
         * with the application object passed in the android_start method.
         *
         * This must be done before instantiating the android display driver.
         */
        class XENGINE_EXPORT AndroidApp {
        public:
            AndroidApp() = delete;

            static android_app *getApp();

            static void setApp(android_app *app);

        private:
            static android_app *app;
        };
    }
}

#endif //XENGINE_ANDROIDAPP_HPP
