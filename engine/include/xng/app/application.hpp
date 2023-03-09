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

            parseArgs(argc, argv);
            loadDrivers();

            window = displayDriver->createWindow(gpuDriverBackend);
            renderDevice = gpuDriver->createRenderDevice();
            audioDevice = audioDriver->createDevice();

            window->update();
        }

        explicit Application(int argc,
                             char *argv[],
                             const std::string &windowTitle,
                             const Vec2i &windowSize,
                             const WindowAttributes &windowAttributes = {}) {
            setCurrentApplication(this);

            parseArgs(argc, argv);
            loadDrivers();

            window = displayDriver->createWindow(gpuDriverBackend, windowTitle, windowSize, windowAttributes);
            renderDevice = gpuDriver->createRenderDevice();
            audioDevice = audioDriver->createDevice();

            window->update();
        }

        virtual ~Application() {
            setCurrentApplication(nullptr);
        }

        virtual int loop() {
            start();
            frameLimiter.reset();
            while (!window->shouldClose() && !shutdown) {
                update(frameLimiter.newFrame());
            }
            stop();
            return 0;
        }

        virtual RenderTarget &getScreenTarget() {
            return window->getRenderTarget();
        }

    protected:
        XENGINE_EXPORT static void setCurrentApplication(Application *ptr);

        DisplayDriverBackend displayDriverBackend = DISPLAY_GLFW;
        GpuDriverBackend gpuDriverBackend = OPENGL_4_6;
        AudioDriverBackend audioDriverBackend = OPENAL_SOFT;

        std::unique_ptr<DisplayDriver> displayDriver = nullptr;
        std::unique_ptr<GpuDriver> gpuDriver = nullptr;
        std::unique_ptr<AudioDriver> audioDriver = nullptr;

        std::unique_ptr<Window> window = nullptr;
        std::unique_ptr<RenderDevice> renderDevice = nullptr;
        std::unique_ptr<AudioDevice> audioDevice = nullptr;

        bool shutdown = false;

        FrameLimiter frameLimiter;

        virtual void start() {}

        virtual void stop() {}

        virtual void update(DeltaTime deltaTime) {
            window->update();
            window->swapBuffers();
        }

        void parseArgs(int argc, char *argv[]) {
            std::vector<std::string> args;
            for (int i = 0; i < argc; i++)
                args.emplace_back(argv[i]);

            for (int i = 0; i < args.size(); i++) {
                if (args.at(i) == "--display") {
                    displayDriverBackend = static_cast<DisplayDriverBackend>(std::stoi(args.at(i + 1)));
                    break;
                }
            }

            for (int i = 0; i < args.size(); i++) {
                if (args.at(i) == "--gpu") {
                    gpuDriverBackend = static_cast<GpuDriverBackend>(std::stoi(args.at(i + 1)));
                    break;
                }
            }

            for (int i = 0; i < args.size(); i++) {
                if (args.at(i) == "--audio") {
                    audioDriverBackend = static_cast<AudioDriverBackend>(std::stoi(args.at(i + 1)));
                    break;
                }
            }
        }

        void loadDrivers() {
            displayDriver = DisplayDriver::load(displayDriverBackend);
            gpuDriver = GpuDriver::load(gpuDriverBackend);
            audioDriver = AudioDriver::load(audioDriverBackend);
        }
    };
}

#endif //XENGINE_APPLICATION_HPP
