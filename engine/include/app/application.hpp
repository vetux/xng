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

#include "compat/imguicompat.hpp"

#include "audio/audiodriver.hpp"
#include "display/displaydriver.hpp"
#include "gpu/gpudriver.hpp"

#include "driver/driverregistry.hpp"

#include "types/deltatime.hpp"

#include "imgui.h"

namespace xng {
    class Application {
    public:
        /**
         * @param argc
         * @param argv
         */
        explicit Application(int argc, char *argv[]) {
            parseArgs(argc, argv);
            loadDrivers();

            window = displayDriver->createWindow(graphicsDriverName);
            renderDevice = graphicsDriver->createRenderDevice();
            audioDevice = audioDriver->createDevice();

            window->update();

            imGuiContext = ImGui::CreateContext();
            ImGuiCompat::Init(*window);
        }

        explicit Application(int argc,
                             char *argv[],
                             const std::string &windowTitle,
                             const Vec2i &windowSize,
                             const WindowAttributes &windowAttributes = {}) {
            parseArgs(argc, argv);
            loadDrivers();

            window = displayDriver->createWindow(graphicsDriverName, windowTitle, windowSize, windowAttributes);
            renderDevice = graphicsDriver->createRenderDevice();
            audioDevice = audioDriver->createDevice();

            window->update();

            imGuiContext = ImGui::CreateContext();
            ImGuiCompat::SetContext(imGuiContext);
            ImGuiCompat::Init(*window);
        }

        virtual ~Application() {
            ImGuiCompat::Shutdown(*window);
            ImGui::DestroyContext(imGuiContext);
        }

        virtual int loop() {
            start();
            auto frameStart = std::chrono::steady_clock::now();
            DeltaTime deltaTime = 0;
            while (!window->shouldClose() && !shutdown) {
                update(deltaTime);
                auto frameEnd = std::chrono::steady_clock::now();
                auto frameDelta = frameEnd - frameStart;
                deltaTime = static_cast<DeltaTime>(static_cast<long double>(frameDelta.count()) / (long double) 1000000000.0f);
                if (targetFramerate > 0) {
                    auto frameDur = std::chrono::steady_clock::duration((int) ((long double)(1.0f / targetFramerate) * (long double)1000000000.0f));
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

    protected:
        std::string displayDriverName = "glfw";
        std::string graphicsDriverName = "opengl";
        std::string audioDriverName = "openal-soft";

        std::unique_ptr<DisplayDriver> displayDriver = nullptr;
        std::unique_ptr<GpuDriver> graphicsDriver = nullptr;
        std::unique_ptr<AudioDriver> audioDriver = nullptr;

        std::unique_ptr<Window> window = nullptr;
        std::unique_ptr<RenderDevice> renderDevice = nullptr;
        std::unique_ptr<AudioDevice> audioDevice = nullptr;

        ImGuiContext *imGuiContext;

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
                    graphicsDriverName = args.at(i + 1);
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
            graphicsDriver = DriverRegistry::load<GpuDriver>(graphicsDriverName);
            audioDriver = DriverRegistry::load<AudioDriver>(audioDriverName);
        }
    };
}

#endif //XENGINE_APPLICATION_HPP
