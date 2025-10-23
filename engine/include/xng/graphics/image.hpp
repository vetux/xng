/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_IMAGE_HPP
#define XENGINE_IMAGE_HPP

#include <vector>
#include <stdexcept>

#include "color.hpp"
#include "xng/resource/resource.hpp"

#include "xng/math/rectangle.hpp"

namespace xng {
    /**
     * Stores 2d image data in row major format.
     *
     * Image coordinate space is as follows:
     *
     *         -y
     *          |
     *     -x ----- +x
     *          |
     *         +y
     *
     * @tparam T The type to use for a pixel
     */
    template<typename T>
    class XENGINE_EXPORT Image final : public Resource {
    public:
        RESOURCE_TYPENAME(Image)

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Image>(*this);
        }

        Image()
            : buffer() {
        }

        Image(const int width, const int height, const std::vector<T> &buffer)
            : resolution(width, height), buffer(buffer) {
        }

        Image(const int width, const int height)
            : resolution(width, height), buffer(width * height) {
        }

        explicit Image(const Vec2i &resolution, ColorRGBA color = ColorRGBA::black())
            : resolution(resolution), buffer(resolution.x * resolution.y, color) {
        }

        Image(const Image &copy)
            : resolution(copy.resolution), buffer(copy.buffer) {
        }

        Image(Image &&other) noexcept
            : resolution(std::move(other.resolution)), buffer(std::move(other.buffer)) {
        }

        ~Image() override = default;

        Image &operator=(const Image &copy) {
            this->resolution = copy.resolution;
            this->buffer = std::vector<T>(copy.buffer);
            return *this;
        }

        Image &operator=(Image &&other) noexcept {
            this->resolution = std::move(other.resolution);
            this->buffer = std::move(other.buffer);
            return *this;
        }

        bool operator()() const {
            return !empty();
        }

        Vec2i getResolution() const { return resolution; }

        const std::vector<T> &getBuffer() const { return buffer; }

        std::vector<T> &getBuffer() { return buffer; }

        int getWidth() const { return resolution.x; }

        int getHeight() const { return resolution.y; }

        bool empty() const { return buffer.empty(); }

        const T &getPixel(const int x, const int y) const {
            return buffer[scanLine(y) + x];
        }

        void setPixel(const int x, const int y, T color) {
            buffer[scanLine(y) + x] = color;
        }

        int scanLine(const int y) const {
            return y * resolution.x;
        }

        void copyRow(const Image &source,
            const int srcRow,
            const int srcColumn,
            const int dstRow,
            const int dstColumn,
            const size_t count) {
            if (source.resolution.y <= srcRow
                || source.resolution.x <= srcColumn
                || source.resolution.x < srcColumn + count
                || resolution.y <= dstRow
                || resolution.x <= dstColumn
                || resolution.x < dstColumn + count) {
                throw std::runtime_error("Invalid copy row / column index or count");
            }

            auto srcOffset = srcRow * source.resolution.x + srcColumn;
            auto dstOffset = dstRow * resolution.x + dstColumn;

            auto begin = source.buffer.begin() + srcOffset;
            std::copy(begin,
                      begin + count,
                      buffer.begin() + dstOffset);
        }

        void copyRows(const Image &source, int srcRow, int dstRow, size_t count) {
            if (resolution != source.resolution
                || source.resolution.y <= srcRow
                || source.resolution.y < srcRow + count
                || resolution.y <= dstRow
                || resolution.y < dstRow + count) {
                throw std::runtime_error("Invalid copy row / column index or count");
            }

            auto srcOffset = srcRow * source.resolution.x;
            auto dstOffset = dstRow * resolution.x;

            auto begin = source.buffer.begin() + srcOffset;
            std::copy(begin,
                      begin + count * resolution.x,
                      buffer.begin() + dstOffset);
        }

        void blit(const Image &source) {
            if (source.resolution.x != resolution.x || source.resolution.y != resolution.y) {
                throw std::runtime_error("Invalid blit source resolution");
            }
            copyRows(source, 0, 0, resolution.y);
        }

        void blit(const Vec2i &targetPosition, const Image &source) {
            if (targetPosition.x < 0
                || targetPosition.y < 0
                || targetPosition.x + source.getWidth() > resolution.x
                || targetPosition.y + source.getHeight() > resolution.y) {
                throw std::runtime_error("Invalid blit rect");
            }
            // Copy rows
            for (int y = 0; y < source.getHeight(); y++) {
                copyRow(source, y, 0, y + targetPosition.y, targetPosition.x, source.getWidth());
            }
        }

        Image slice(const Recti &rect) const {
            Image ret = Image(rect.dimensions.x, rect.dimensions.y);
            for (int x = rect.position.x; x < rect.position.x + rect.dimensions.x; x++) {
                for (int y = rect.position.y; y < rect.position.y + rect.dimensions.y; y++) {
                    ret.setPixel(x - rect.position.x, y - rect.position.y, getPixel(x, y));
                }
            }
            return std::move(ret);
        }

        Image swapRows() {
            Image ret = Image(resolution.x, resolution.y);
            for (int y = 0; y < resolution.y; y++) {
                for (int x = 0; x < resolution.x; x++) {
                    ret.setPixel(resolution.x - 1 - x, y, getPixel(x, y));
                }
            }
            return std::move(ret);
        }

        Image swapColumns() {
            Image ret = Image(resolution.x, resolution.y);
            for (auto y = 0; y < resolution.y; y++) {
                ret.copyRow(*this, y, 0, resolution.y - y - 1, 0, resolution.x);
            }
            return std::move(ret);
        }

    protected:
        Vec2i resolution;
        std::vector<T> buffer;
    };

    typedef Image<ColorRGBA> ImageRGBA;
    typedef Image<ColorRGB> ImageRGB;
    typedef Image<float> ImageFloat;
}

#endif //XENGINE_IMAGE_HPP
