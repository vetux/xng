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

#ifndef XENGINE_HEAPMAPPING_HPP
#define XENGINE_HEAPMAPPING_HPP

#include <algorithm>
#include <memory>
#include <stdexcept>

namespace xng {
    class HeapMapping {
    public:
        virtual ~HeapMapping() = default;

        virtual uint8_t *data() = 0;

        virtual size_t size() = 0;

        uint8_t *begin() {
            return data();
        }

        uint8_t *end() {
            return data() + size();
        }

        uint8_t &operator[](const size_t i) {
            if (i >= size()) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping read");
            }
            return data()[i];
        }

        [[nodiscard]] bool empty() {
            return size() == 0;
        }

        void copyFrom(const std::vector<uint8_t> &src,
                      const size_t srcOffset,
                      const size_t dstOffset,
                      const size_t count) {
            if (srcOffset + count > src.size()) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping copy");
            }
            if (dstOffset + count > size()) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping copy");
            }
            std::copy_n(src.begin() + static_cast<ptrdiff_t>(srcOffset),
                        count,
                        data() + static_cast<ptrdiff_t>(dstOffset));
        }

        void copyTo(std::vector<uint8_t> &dst,
                    const size_t srcOffset,
                    const size_t dstOffset,
                    const size_t count) {
            if (srcOffset + count > size()) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping copy");
            }
            if (dstOffset + count > dst.size()) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping copy");
            }
            std::copy_n(data() + static_cast<ptrdiff_t>(srcOffset),
                        count,
                        dst.begin() + static_cast<ptrdiff_t>(dstOffset));
        }

        std::vector<uint8_t> copyTo(const size_t srcOffset, const size_t count) {
            if (srcOffset + count > size()) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping copy");
            }
            std::vector<uint8_t> dst(count);
            std::copy_n(data() + static_cast<ptrdiff_t>(srcOffset),
                        count,
                        dst.begin());
            return dst;
        }
    };
}

#endif //XENGINE_HEAPMAPPING_HPP
