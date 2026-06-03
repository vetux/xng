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

#ifndef XENGINE_TILELOADER_HPP
#define XENGINE_TILELOADER_HPP

#include <cstdint>
#include <vector>

#include "xng/math/vector2.hpp"
#include "xng/renderer/samplingproperties.hpp"

namespace xng {
    /**
     * For each logical image there is one tile loader.
     *
     * For rapid iteration the tile loader can be backed by tiles stored in RAM.
     * In production the tile loader will read pre-cooked tiles from disk.
     */
    class TileLoader {
    public:
        virtual ~TileLoader() = default;

        /**
         * @return The size of mip level 0.
         */
        virtual const Vec2i &getSize() = 0;

        /**
         * Each mip level size must be rg::Texture::getMipLevelSize(getSize(), mipN).
         * There must not be more than rg::Texture::calculateMipLevels(getSize()) mip levels.
         *
         * @return The maximum allocated mip index
         */
        virtual unsigned int getMaxMip() = 0;

        /**
         * @return The wrapping method to use for this image.
         */
        virtual WrappingMethod getWrappingMethod() = 0;

        /**
         * This method can be called from any thread, so it must be threadsafe.
         *
         * @param mipLevel
         * @param tile
         * @return The tile texels including the atlas border.
         */
        virtual std::vector<uint8_t> getTile(int mipLevel, Vec2i tile) = 0;
    };
}

#endif //XENGINE_TILELOADER_HPP