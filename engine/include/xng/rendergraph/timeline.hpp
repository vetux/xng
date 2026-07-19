/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_TIMELINE_HPP
#define XENGINE_TIMELINE_HPP

#include <chrono>
#include <string>
#include <vector>

namespace xng::rg {
    class Timeline {
    public:
        /**
         * A timeline slice containing the start / end time stamps of a pass on the device timeline.
         */
        struct Slice {
            std::string passName;
            std::chrono::nanoseconds start{};
            std::chrono::nanoseconds end{};

            Slice() = default;

            Slice(std::string passName,
                  const std::chrono::nanoseconds start,
                  const std::chrono::nanoseconds end)
                : passName(std::move(passName)), start(start), end(end) {
            }
        };

        // The time point in the host time domain at which the first slice was submitted.
        // This is exposed here because on certain platforms it might be possible to fetch both host and device time atomically.
        std::chrono::high_resolution_clock::time_point submitTimeHost;

        // The time point in the device time domain at which the first slice was submitted. (Slices can span multiple submissions)
        std::chrono::nanoseconds submitTimeDevice;

        // The time stamps in the device time domain.
        std::vector<Slice> slices{};
    };
}

#endif //XENGINE_TIMELINE_HPP
