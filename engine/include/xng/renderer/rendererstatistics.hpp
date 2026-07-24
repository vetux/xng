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

#ifndef XENGINE_RENDERER_STATISTICS_HPP
#define XENGINE_RENDERER_STATISTICS_HPP

#include <string>

namespace xng {
    /**
     * Optional statistics data for debugging only.
     * May not be assigned or contain inaccurate data.
     */
    struct RendererStatistics {
        std::string vendor{};
        std::string renderer{};
        std::string version{};

        size_t drawCalls = 0;
        size_t polygons = 0;

        size_t bufferVRamUsage = 0;
        size_t bufferVRamUpload = 0;
        size_t bufferVRamDownload = 0;
        size_t bufferVRamCopy = 0;

        size_t textureVRamUsage = 0;
        size_t textureVRamUpload = 0;
        size_t textureVRamDownload = 0;
        size_t textureVRamCopy = 0;

        size_t tilesInFlight = 0;

        std::chrono::high_resolution_clock::time_point frameStart;
        std::chrono::high_resolution_clock::time_point frameSubmit;
        std::chrono::high_resolution_clock::time_point frameEnd;
        std::vector<std::pair<std::string, std::chrono::nanoseconds>> gpuTime;

        size_t streamingBudgetMax;
        size_t streamingBudgetUsed;
    };
}

#endif //XENGINE_RENDERER_STATISTICS_HPP
