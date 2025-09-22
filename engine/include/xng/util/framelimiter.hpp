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

#ifndef XENGINE_FRAMELIMITER_HPP
#define XENGINE_FRAMELIMITER_HPP

#include <chrono>
#include <thread>

#include "xng/util/time.hpp"

namespace xng {
    class FrameLimiter {
    public:
        FrameLimiter() = default;

        explicit FrameLimiter(const int targetFrameRate)
            : deltaTime() {
            targetFrameDuration = std::chrono::round<std::chrono::steady_clock::duration>(
                std::chrono::duration<double>(1.0 / targetFrameRate));
        }

        explicit FrameLimiter(const std::chrono::steady_clock::duration targetFrameDuration)
            : targetFrameDuration(targetFrameDuration), deltaTime() {
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
            if (targetFrameDuration.count() > 0 && now < start + targetFrameDuration) {
                constexpr auto spin_threshold = std::chrono::microseconds(200);
                if (now + spin_threshold < deadline) {
                    std::this_thread::sleep_until(deadline - spin_threshold);
                }
                while (std::chrono::steady_clock::now() < deadline) {
                }
                now = std::chrono::steady_clock::now();
            }
            deltaTime = now - start;
            start = now;
            deadline = now + targetFrameDuration;
            auto v = std::chrono::steady_clock::duration::period::den;
            return DeltaTime(static_cast<DeltaTime>(deltaTime.count()) / static_cast<DeltaTime>(v));
        }

        const std::chrono::steady_clock::duration &getDeltaTime() const {
            return deltaTime;
        }

        unsigned int getFramerate() const {
            return std::ceil(1.0 / std::chrono::duration_cast<std::chrono::duration<double> >(deltaTime).count());
        }

        void setTargetFrameRate(const int frameRate) {
            targetFrameDuration = std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                std::chrono::duration<double>(1.0 / frameRate));
        }

        void setTargetFrameDuration(const std::chrono::steady_clock::duration &duration) {
            targetFrameDuration = duration;
        }

    private:
        std::chrono::steady_clock::duration targetFrameDuration{};
        std::chrono::steady_clock::duration deltaTime;
        std::chrono::steady_clock::time_point start;
        std::chrono::steady_clock::time_point deadline;
    };
}

#endif //XENGINE_FRAMELIMITER_HPP
