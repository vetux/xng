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

#ifndef XENGINE_ECSPROFILER_HPP
#define XENGINE_ECSPROFILER_HPP

#include <chrono>
#include <mutex>

#include "xng/ecs/profiling/ecsframelist.hpp"

namespace xng {
    class ECSProfiler {
    public:
        ECSProfiler() = default;

        ECSProfiler(const ECSProfiler &other) {
            frames = other.frames;
        }

        ECSProfiler &operator=(const ECSProfiler &other) {
            frames = other.frames;
            return *this;
        }

        ECSProfiler(ECSProfiler &&other)  noexcept {
            frames = std::move(other.frames);
        }

        ECSProfiler &operator=(ECSProfiler &&other)  noexcept {
            frames = std::move(other.frames);
            return *this;
        }

        void beginFrame() {
            frameStart = std::chrono::high_resolution_clock::now();
            frames = ECSFrameList();
        }

        void beginPipelineUpdate() {
            pipelineStart = std::chrono::high_resolution_clock::now();
        }

        int beginSystemUpdate() {
            return startSystemUpdate();
        }

        void endSystemUpdate(const std::string &system, int id) {
            auto systemEnd = std::chrono::high_resolution_clock::now();

            mutex.lock();

            ECSSample sample;
            sample.systemName = system;
            sample.time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    systemEnd - systemStarts.at(id)).count();
            frameSamples.emplace_back(sample);

            endSystemUpdate(id);

            mutex.unlock();
        }

        void endPipelineUpdate(const std::string &pipeline) {
            auto frameEnd = std::chrono::high_resolution_clock::now();

            ECSFrame frame;
            frame.pipeline = pipeline;
            frame.duration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
            frame.samples = frameSamples;
            frames.addFrame(frame);

            frameSamples.clear();
        }

        void endFrame() {
            auto frameEnd = std::chrono::high_resolution_clock::now();
            frames.totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
        }

        const ECSFrameList &getFrames() const {
            return frames;
        }

    private:
        int startSystemUpdate() {
            auto time = std::chrono::high_resolution_clock::now();
            mutex.lock();
            int ret;
            if (systemStartCounterCache.empty()) {
                if (std::numeric_limits<int>::max() == systemStartCounter) {
                    throw std::runtime_error("Counter overflow");
                }
                ret = systemStartCounter++;
            } else {
                ret = *systemStartCounterCache.begin();
                systemStartCounterCache.erase(systemStartCounterCache.begin());
            }
            systemStarts[ret] = time;
            mutex.unlock();
            return ret;
        }

        void endSystemUpdate(int id) {
            systemStarts.erase(id);
            systemStartCounterCache.insert(id);
        }

        std::mutex mutex;

        int systemStartCounter = 0;
        std::set<int> systemStartCounterCache;
        std::map<int, std::chrono::high_resolution_clock::time_point> systemStarts;

        std::chrono::high_resolution_clock::time_point frameStart;

        std::chrono::high_resolution_clock::time_point pipelineStart;

        std::vector<ECSSample> frameSamples;

        ECSFrameList frames;
    };
}
#endif //XENGINE_ECSPROFILER_HPP
