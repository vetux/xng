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

#ifndef XENGINE_FRAMEGRAPHUPLOADBUFFER_HPP
#define XENGINE_FRAMEGRAPHUPLOADBUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

namespace xng {
    struct FrameGraphUploadBuffer {
        std::vector<uint8_t> data;

        FrameGraphUploadBuffer() = default;

        FrameGraphUploadBuffer(size_t size, const uint8_t *v) : data(v, v + size) {}

        template<typename T>
        static FrameGraphUploadBuffer createValue(const T &value) {
            return {sizeof(T), reinterpret_cast<const uint8_t *>(&value)};
        }

        template<typename T>
        static FrameGraphUploadBuffer createArray(const std::vector<T> &value) {
            return {sizeof(T) * value.size(),
                    reinterpret_cast<const uint8_t *>(value.data())};
        }
    };
}

#endif //XENGINE_FRAMEGRAPHUPLOADBUFFER_HPP
