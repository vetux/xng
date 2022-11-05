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

#ifndef XENGINE_APPLICATION_HPP
#define XENGINE_APPLICATION_HPP

#include <chrono>
#include <thread>
#include <iomanip>

#include "xng/audio/audiodriver.hpp"
#include "xng/display/displaydriver.hpp"
#include "xng/gpu/gpudriver.hpp"

#include "xng/driver/driverregistry.hpp"

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

            window = displayDriver->createWindow(gpuDriverName);
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

            window = displayDriver->createWindow(gpuDriverName, windowTitle, windowSize, windowAttributes);
            renderDevice = gpuDriver->createRenderDevice();
            audioDevice = audioDriver->createDevice();

            window->update();
        }

        virtual ~Application() {
            setCurrentApplication(nullptr);
        }

        virtual int loop() {
            start();
            auto frameStart = std::chrono::steady_clock::now();
            DeltaTime deltaTime = 0;
            while (!window->shouldClose() && !shutdown) {
                update(deltaTime);
                auto frameEnd = std::chrono::steady_clock::now();
                auto frameDelta = frameEnd - frameStart;
                deltaTime = static_cast<DeltaTime>(static_cast<long double>(frameDelta.count()) /
                                                   (long double) 1000000000.0f);
                if (targetFramerate > 0) {
                    auto frameDur = std::chrono::steady_clock::duration(
                            (int) ((long double) (1.0f / targetFramerate) * (long double) 1000000000.0f));
                    if (frameDelta < frameDur) {
                        std::this_thread::sleep_for(frameDur - frameDelta);
                    }
                    frameEnd = std::chrono::steady_clock::now();
                    frameDelta = frameEnd - frameStart;
                    deltaTime = static_cast<DeltaTime>(static_cast<long double>(frameDelta.count()) /
                                                       (long double) 1000000000.0f);
                }
                frameStart = frameEnd;
            }
            stop();
            return 0;
        }

        virtual std::string getDisplayDriverName() const {
            return displayDriverName;
        }

        virtual const std::string &getGpuDriverName() const {
            return gpuDriverName;
        }

        virtual const std::string &getAudioDriverName() const {
            return audioDriverName;
        }

        virtual DisplayDriver &getDisplayDriver() {
            if (!displayDriver) {
                throw std::runtime_error("Display driver not initialized");
            }
            return *displayDriver;
        }

        virtual GpuDriver &getGpuDriver() {
            if (!gpuDriver) {
                throw std::runtime_error("Gpu driver not initialized");
            }
            return *gpuDriver;
        }

        virtual AudioDriver &getAudioDriver() {
            if (!audioDriver) {
                throw std::runtime_error("Audio driver not initialized");
            }
            return *audioDriver;
        }

        virtual Window &getWindow() {
            if (!window) {
                throw std::runtime_error("Window not initialized");
            }
            return *window;
        }

        virtual RenderDevice &getRenderDevice() {
            if (!renderDevice) {
                throw std::runtime_error("Render device not initialized");
            }
            return *renderDevice;
        }

        virtual AudioDevice &getAudioDevice() {
            if (!audioDevice) {
                throw std::runtime_error("Audio device not initialized");
            }
            return *audioDevice;
        }

        virtual RenderTarget &getScreenTarget() {
            return window->getRenderTarget();
        }

    protected:
        XENGINE_EXPORT static void setCurrentApplication(Application *ptr);

        std::string displayDriverName = "glfw";
        std::string gpuDriverName = "opengl";
        std::string audioDriverName = "openal-soft";

        std::unique_ptr<DisplayDriver> displayDriver = nullptr;
        std::unique_ptr<GpuDriver> gpuDriver = nullptr;
        std::unique_ptr<AudioDriver> audioDriver = nullptr;

        std::unique_ptr<Window> window = nullptr;
        std::unique_ptr<RenderDevice> renderDevice = nullptr;
        std::unique_ptr<AudioDevice> audioDevice = nullptr;

        bool shutdown = false;

        long double targetFramerate = 0;

        virtual void start() {}

        virtual void stop() {}

        virtual void update(DeltaTime deltaTime) {
            window->update();
            window->swapBuffers();
        }

        virtual void parseArgs(int argc, char *argv[]) {
            std::vector<std::string> args;
            for (int i = 0; i < argc; i++)
                args.emplace_back(argv[i]);

            for (int i = 0; i < args.size(); i++) {
                if (args.at(i) == "--display") {
                    displayDriverName = args.at(i + 1);
                }
            }

            for (int i = 0; i < args.size(); i++) {
                if (args.at(i) == "--graphics") {
                    gpuDriverName = args.at(i + 1);
                }
            }

            for (int i = 0; i < args.size(); i++) {
                if (args.at(i) == "--audio") {
                    audioDriverName = args.at(i + 1);
                }
            }
        }

        virtual void loadDrivers() {
            displayDriver = DriverRegistry::load<DisplayDriver>(displayDriverName);
            gpuDriver = DriverRegistry::load<GpuDriver>(gpuDriverName);
            audioDriver = DriverRegistry::load<AudioDriver>(audioDriverName);
        }
    };
}

#endif //XENGINE_APPLICATION_HPP
