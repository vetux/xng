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

#ifndef XENGINE_ECSPROFILER_HPP
#define XENGINE_ECSPROFILER_HPP

#include <chrono>

#include "ecs/profiling/ecsframelist.hpp"

namespace xng {
    class ECSProfiler {
    public:
        void beginFrame() {
            frameStart = std::chrono::high_resolution_clock::now();
        }

        void beginSystemUpdate() {
            systemStart = std::chrono::high_resolution_clock::now();
        }

        void endSystemUpdate(const std::string &system) {
            auto systemEnd = std::chrono::high_resolution_clock::now();

            ECSSample sample;
            sample.systemName = system;
            sample.time = std::chrono::duration_cast<std::chrono::milliseconds>(systemEnd - systemStart).count();
            frameSamples.emplace_back(sample);
        }

        void endFrame() {
            auto frameEnd = std::chrono::high_resolution_clock::now();

            ECSFrame frame;
            frame.duration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
            frame.samples = frameSamples;
            frames.addFrame(frame);

            frameSamples.clear();
        }

        const ECSFrameList &getFrames() const {
            return frames;
        }

    private:
        std::chrono::high_resolution_clock::time_point frameStart;

        std::chrono::high_resolution_clock::time_point systemStart;
        std::vector<ECSSample> frameSamples;

        ECSFrameList frames;
    };
}
#endif //XENGINE_ECSPROFILER_HPP
