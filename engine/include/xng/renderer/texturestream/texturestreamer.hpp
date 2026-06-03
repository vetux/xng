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

#ifndef XENGINE_TEXTURESTREAMER_HPP
#define XENGINE_TEXTURESTREAMER_HPP

#include "xng/assets/image.hpp"
#include "xng/math/vector2.hpp"

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/resource/texture.hpp"

#include "xng/renderer/stream/streambuffer.hpp"
#include "xng/renderer/stream/streamtexture.hpp"

namespace xng {
    /**
     * The TextureStreamer manages TileStreamers for each render texture in the scene.
     * It samples from the readback buffer to determine which tiles to stream in via the TileStreamers.
     */
    class TextureStreamer {
    public:

    };
}

#endif //XENGINE_TEXTURESTREAMER_HPP
