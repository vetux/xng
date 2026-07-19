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
        typedef unsigned int TimeStamp;

        /**
         * A timeline slice containing the start / end time stamps of a pass on the gpu timeline.
         */
        struct Slice {
            std::string passName;
            TimeStamp start{};
            TimeStamp end{};

            Slice() = default;

            Slice(std::string passName,
                  const TimeStamp start,
                  const TimeStamp end)
                : passName(std::move(passName)), start(start), end(end) {
            }
        };

        // The time point in the gpu time domain at which this timeline was submitted to the gpu.
        TimeStamp submitTime;

        // The time point in the gpu time domain at which this timeline finished execution.
        TimeStamp finishTime;

        // The time stamps.
        std::vector<Slice> slices{};
    };
}

#endif //XENGINE_TIMELINE_HPP
