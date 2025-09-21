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

#ifndef XENGINE_FRAMELIMITER_HPP
#define XENGINE_FRAMELIMITER_HPP

#include <chrono>
#include <thread>

#include "xng/util/time.hpp"

namespace xng {
    class FrameLimiter {
    public:
        FrameLimiter() = default;

        explicit FrameLimiter(int targetFrameRate)
            : targetFrameDuration(std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                std::chrono::milliseconds(1000 / targetFrameRate))) {
        }

        explicit FrameLimiter(std::chrono::milliseconds targetFrameDuration)
            : targetFrameDuration(
                std::chrono::duration_cast<std::chrono::steady_clock::duration>(targetFrameDuration)) {
        }

        /**
         * Should be called before the first call to newFrame to beginFrame the start time point
         */
        void reset() {
            start = std::chrono::steady_clock::now();
        }

        /**
         * Should be called at the beginning of every new frame.
         *
         * Calculates the delta time between this frame and the previous frame and if the target duration
         * is set suspends the calling thread for the time difference between the target duration and the
         * frame duration to limit the framerate.
         *
         * @return The time delta between the last invocation of newFrame or reset and this call to newFrame.
         */
        DeltaTime newFrame() {
            auto now = std::chrono::steady_clock::now();
            deltaTime = now - start;
            if (targetFrameDuration.count() > 0 && deltaTime < targetFrameDuration) {
                auto diff = targetFrameDuration - deltaTime;
                std::this_thread::sleep_for(diff);
            }
            now = std::chrono::steady_clock::now();
            deltaTime = now - start;
            start = now;
            fpsAverage = fpsAlpha * fpsAverage + (1.0 - fpsAlpha) * getFramerate();
            auto v = std::chrono::steady_clock::duration::period::den;
            return DeltaTime(static_cast<DeltaTime>(deltaTime.count()) / static_cast<DeltaTime>(v));
        }

        const std::chrono::steady_clock::duration &getDeltaTime() const {
            return deltaTime;
        }

        unsigned int getFramerate() const {
            return static_cast<unsigned int>(std::round(1000000000.0f / static_cast<float>(deltaTime.count())));
        }

        unsigned int getAverageFramerate() const {
            return fpsAverage;
        }

        void setTargetFrameRate(int frameRate) {
            targetFrameDuration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                std::chrono::milliseconds(1000 / frameRate));
        }

        void setTargetFrameDuration(const std::chrono::steady_clock::duration &duration) {
            targetFrameDuration = duration;
        }

    private:
        unsigned int fpsAverage = 0;
        float fpsAlpha = 0.9f;
        std::chrono::steady_clock::duration targetFrameDuration{};
        std::chrono::steady_clock::duration deltaTime;
        std::chrono::steady_clock::time_point start;
    };
}

#endif //XENGINE_FRAMELIMITER_HPP
