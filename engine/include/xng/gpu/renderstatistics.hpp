/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_RENDERSTATISTICS_HPP
#define XENGINE_RENDERSTATISTICS_HPP

namespace xng {
    struct RenderStatistics {
        size_t binds{}; // The number of binding operations
        size_t drawCalls{}; // The number of draw calls
        size_t polys{}; // The number of polygons

        // I/O Stats
        size_t uploadVertex{};
        size_t uploadIndex{};
        size_t uploadTexture{};
        size_t downloadTexture{};
        size_t uploadShaderStorage{};
        size_t downloadShaderStorage{};
        size_t uploadShaderUniform{};
        size_t uploadCommand{};

        size_t getTotalUpload() const {
            return uploadVertex + uploadIndex + uploadTexture + uploadShaderStorage + uploadShaderUniform + uploadCommand;
        }

        size_t getTotalDownload()const {
            return downloadTexture + downloadShaderStorage;
        }
    };
}
#endif //XENGINE_RENDERSTATISTICS_HPP
