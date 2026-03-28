/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_RENDERGRAPH_STATISTICS_HPP
#define XENGINE_RENDERGRAPH_STATISTICS_HPP

#include <string>

namespace xng::rendergraph {
    /**
     * Optional statistics data for debugging only.
     * May not be assigned or contain inaccurate data.
     */
    struct Statistics {
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

        Statistics &operator+=(const Statistics &other) {
            drawCalls += other.drawCalls;
            polygons += other.polygons;

            bufferVRamUsage += other.bufferVRamUsage;
            bufferVRamUpload += other.bufferVRamUpload;
            bufferVRamDownload += other.bufferVRamDownload;
            bufferVRamCopy += other.bufferVRamCopy;

            textureVRamUsage += other.textureVRamUsage;
            textureVRamUpload += other.textureVRamUpload;
            textureVRamDownload += other.textureVRamDownload;
            textureVRamCopy += other.textureVRamCopy;

            return *this;
        }
    };
}

#endif //XENGINE_RENDERGRAPH_STATISTICS_HPP