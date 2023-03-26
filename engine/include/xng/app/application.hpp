/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_APPLICATION_HPP
#define XENGINE_APPLICATION_HPP

#include <chrono>
#include <thread>
#include <iomanip>

#include "xng/audio/audiodriver.hpp"
#include "xng/display/displaydriver.hpp"
#include "xng/gpu/gpudriver.hpp"
#include "xng/util/framelimiter.hpp"

#include "xng/types/deltatime.hpp"

namespace xng {
    class Application {
    public:
        XENGINE_EXPORT static Application &getCurrentApplication();

        /**
         * @param argc
         * @param argv
         */
        explicit Application(int argc, char *argv[]) {
            setCurrentApplication(this);
        }

        virtual ~Application() {
            setCurrentApplication(nullptr);
        }

        virtual int loop() {
            start();
            frameLimiter.reset();
            while (!shutdown) {
                update(frameLimiter.newFrame());
            }
            stop();
            return 0;
        }

    protected:
        XENGINE_EXPORT static void setCurrentApplication(Application *ptr);

        bool shutdown = false;

        FrameLimiter frameLimiter;

        virtual void start() {}

        virtual void stop() {}

        virtual void update(DeltaTime deltaTime) {}
    };
}

#endif //XENGINE_APPLICATION_HPP
