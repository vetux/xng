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

#ifndef XENGINE_HEAPMAPPING_HPP
#define XENGINE_HEAPMAPPING_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cassert>
#include <functional>
#include <stdexcept>

namespace xng {
    class HeapMapping {
    public:
        HeapMapping() = default;

        HeapMapping(uint8_t *blockPtr, const size_t blockSize, std::function<void()> deleter)
            : blockPtr(blockPtr), blockSize(blockSize), deleter(std::move(deleter)) {
            assert(blockPtr != nullptr);
        }

        ~HeapMapping() {
            if (deleter) {
                deleter();
            }
        }

        HeapMapping(const HeapMapping &) = delete;

        HeapMapping &operator=(const HeapMapping &) = delete;

        HeapMapping(HeapMapping &&other) noexcept
            : blockPtr(other.blockPtr), blockSize(other.blockSize), deleter(std::move(other.deleter)) {
            other.blockPtr = nullptr;
            other.blockSize = 0;
            other.deleter = {};
        }

        HeapMapping &operator=(HeapMapping &&other) noexcept {
            if (this == &other) return *this;
            blockPtr = other.blockPtr;
            blockSize = other.blockSize;
            deleter = std::move(other.deleter);
            other.blockPtr = nullptr;
            other.blockSize = 0;
            other.deleter = {};
            return *this;
        }

        uint8_t *begin() {
            return blockPtr;
        }

        uint8_t *end() {
            return blockPtr + blockSize;
        }

        [[nodiscard]] const uint8_t *begin() const {
            return blockPtr;
        }

        [[nodiscard]] const uint8_t *end() const {
            return blockPtr + blockSize;
        }

        uint8_t &operator[](const size_t i) {
            if (i >= blockSize) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping read");
            }
            return blockPtr[i];
        }

        const uint8_t &operator[](const size_t i) const {
            if (i >= blockSize) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping read");
            }
            return blockPtr[i];
        }

        uint8_t *data() {
            return blockPtr;
        }

        [[nodiscard]] const uint8_t *data() const {
            return blockPtr;
        }

        [[nodiscard]] size_t size() const {
            return blockSize;
        }

        [[nodiscard]] bool empty() const {
            return blockSize == 0;
        }

        void copyFrom(const std::vector<uint8_t> &src,
                      const size_t srcOffset,
                      const size_t dstOffset,
                      const size_t count) const {
            if (srcOffset + count > src.size()) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping copy");
            }
            if (dstOffset + count > blockSize) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping copy");
            }
            std::copy_n(src.begin() + static_cast<ptrdiff_t>(srcOffset),
                        count,
                        blockPtr + static_cast<ptrdiff_t>(dstOffset));
        }

        void copyTo(std::vector<uint8_t> &dst,
                    const size_t srcOffset,
                    const size_t dstOffset,
                    const size_t count) const {
            if (srcOffset + count > blockSize) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping copy");
            }
            if (dstOffset + count > dst.size()) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping copy");
            }
            std::copy_n(blockPtr + static_cast<ptrdiff_t>(srcOffset),
                        count,
                        dst.begin() + static_cast<ptrdiff_t>(dstOffset));
        }

        [[nodiscard]] std::vector<uint8_t> copyTo(const size_t srcOffset, const size_t count) const {
            if (srcOffset + count > blockSize) {
                throw std::runtime_error("Attempted to perform an out of bounds heap mapping copy");
            }
            std::vector<uint8_t> dst(count);
            std::copy_n(blockPtr + static_cast<ptrdiff_t>(srcOffset),
                        count,
                        dst.begin());
            return dst;
        }

    private:
        uint8_t *blockPtr = nullptr;
        size_t blockSize = 0;
        std::function<void()> deleter = {};
    };
}

#endif //XENGINE_HEAPMAPPING_HPP
