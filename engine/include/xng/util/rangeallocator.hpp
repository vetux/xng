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

#ifndef XENGINE_RANGEALLOCATOR_HPP
#define XENGINE_RANGEALLOCATOR_HPP

#include <cassert>
#include <cstddef>
#include <map>

namespace xng {
    class RangeAllocator {
    public:
        explicit RangeAllocator(const size_t initialSize = 0)
            : rangeCount(initialSize) {
            freeRanges.emplace(0, initialSize);
        }

        bool hasFreeRange(const size_t count) const {
            size_t offset;
            return getFreeRange(count, offset);
        }

        bool allocate(const size_t count, size_t &out) {
            out = rangeCount;
            if (getFreeRange(count, out)) {
                const auto it = freeRanges.find(out);
                assert(it != freeRanges.end());
                it->second -= count;
                out = it->first + it->second;
                if (it->second == 0) {
                    freeRanges.erase(it);
                }
                return true;
            }
            return false;
        }

        size_t allocate(const size_t count) {
            size_t ret = rangeCount;
            if (getFreeRange(count, ret)) {
                const auto it = freeRanges.find(ret);
                assert(it != freeRanges.end());
                it->second -= count;
                ret = it->first + it->second;
                if (it->second == 0) {
                    freeRanges.erase(it);
                }
            } else {
                rangeCount += count;
            }
            return ret;
        }

        void free(const size_t index, const size_t count) {
            // Insert freed range
            size_t targetRange = index;
            bool foundFreeRange = false;
            for (auto &pair: freeRanges) {
                if (pair.first + pair.second == index) {
                    pair.second += count;
                    foundFreeRange = true;
                    targetRange = pair.first;
                    break;
                }
            }

            if (!foundFreeRange) {
                freeRanges.emplace(index, count);
            }

            // bidirectional merge
            auto it = freeRanges.find(targetRange);
            assert(it != freeRanges.end());

            if (it != freeRanges.begin()) {
                auto prev = std::prev(it);
                auto next = std::next(it);
                if (next != freeRanges.end()) {
                    if (prev->first + prev->second == it->first
                        && next->first == it->first + it->second) {
                        // bidirectional merge
                        prev->second += it->second + next->second;
                        freeRanges.erase(next);
                        freeRanges.erase(targetRange);
                    } else if (prev->first + prev->second == it->first) {
                        // merge left
                        prev->second += it->second;
                        freeRanges.erase(targetRange);
                    } else if (next->first == it->first + it->second) {
                        // merge right
                        it->second += next->second;
                        freeRanges.erase(next);
                    }
                } else {
                    if (prev->first + prev->second == it->first) {
                        // merge left
                        prev->second += it->second;
                        freeRanges.erase(targetRange);
                    }
                }
            } else {
                auto next = std::next(it);
                if (next != freeRanges.end()) {
                    if (next->first == it->first + it->second) {
                        // merge right
                        it->second += next->second;
                        freeRanges.erase(next);
                    }
                }
            }
        }

        [[nodiscard]] size_t getSize() const {
            return rangeCount;
        }

    private:
        bool getFreeRange(const size_t count, size_t &out) const {
            for (auto &pair: freeRanges) {
                if (pair.second >= count) {
                    out = pair.first;
                    return true;
                }
            }
            return false;
        }

        std::map<size_t, size_t> freeRanges;
        size_t rangeCount = 0;
    };
}

#endif //XENGINE_RANGEALLOCATOR_HPP
