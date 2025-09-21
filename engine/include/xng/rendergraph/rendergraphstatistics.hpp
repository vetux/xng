/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_RENDERGRAPHSTATISTICS_HPP
#define XENGINE_RENDERGRAPHSTATISTICS_HPP

#include <cstddef>

namespace xng {
    /**
     * Optional statistics data for debugging only.
     * May not be assigned or contain inaccurate data.
     */
    struct RenderGraphStatistics {
        size_t drawCalls = 0;
        size_t polygons = 0;

        size_t vertexVRamUsage = 0;
        size_t vertexVRamUpload = 0;
        size_t vertexVRamDownload = 0;
        size_t vertexVRamCopy = 0;

        size_t indexVRamUsage = 0;
        size_t indexVRamUpload = 0;
        size_t indexVRamDownload = 0;
        size_t indexVRamCopy = 0;

        size_t textureVRamUsage = 0;
        size_t textureVRamUpload = 0;
        size_t textureVRamDownload = 0;
        size_t textureVRamCopy = 0;

        size_t shaderBufferVRamUsage = 0;
        size_t shaderBufferVRamUpload = 0;
        size_t shaderBufferVRamDownload = 0;
        size_t shaderBufferVRamCopy = 0;
    };
}

#endif //XENGINE_RENDERGRAPHSTATISTICS_HPP