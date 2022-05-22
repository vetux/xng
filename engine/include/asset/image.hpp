/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_IMAGE_HPP
#define XENGINE_IMAGE_HPP

#include <vector>
#include <stdexcept>
#include <typeindex>

#include "asset/color.hpp"
#include "resource/resource.hpp"

#include "math/rectangle.hpp"

namespace xengine {
    /**
     * Stores 2d image data in row major format.
     *
     * @tparam T The type to use for a pixel
     */
    template<typename T>
    class XENGINE_EXPORT Image : public Resource {
    public:
        Resource *clone() override {
            return new Image<T>(*this);
        }

        std::type_index getTypeIndex() override {
            return typeid(Image<T>);
        }

        Image() : size(), buffer() {}

        Image(int width, int height, const std::vector<T> &buffer) : size(width, height),
                                                                     buffer(buffer) {}

        Image(int width, int height) : size(width, height), buffer(width * height) {}

        explicit Image(Vec2i size) : size(size), buffer(size.x * size.y) {}

        Image(const Image &copy) : size(copy.size), buffer(copy.buffer) {}

        Image(Image &&other) noexcept: size(std::move(other.size)), buffer(std::move(other.buffer)) {}

        ~Image() override = default;

        Image &operator=(const Image &copy) {
            this->size = copy.size;
            this->buffer = std::vector<T>(copy.buffer);
            return *this;
        }

        Image &operator=(Image &&other) noexcept {
            this->size = std::move(other.size);
            this->buffer = std::move(other.buffer);
            return *this;
        }

        bool operator()() const {
            return !empty();
        }

        Vec2i getSize() const { return size; }

        const T *getData() const { return buffer.data(); }

        T *getData() { return buffer.data(); }

        int getWidth() const { return size.x; }

        int getHeight() const { return size.y; }

        bool empty() const { return buffer.empty(); }

        const T &getPixel(int x, int y) const {
            return buffer[scanLine(y) + x];
        }

        void setPixel(int x, int y, T color) {
            buffer[scanLine(y) + x] = color;
        }

        int scanLine(int y) const {
            return y * size.x;
        }

        void blit(const Image<T> &source) {
            if (source.size.x != size.x || source.height != size.y) {
                throw std::runtime_error("Invalid blit source size");
            }
            for (int x = 0; x < size.x; x++) {
                for (int y = 0; y < size.y; y++) {
                    setPixel(x, y, source.getPixel(x, y));
                }
            }
        }

        void blit(const Recti &targetRect, const Image<T> &source) {
            if (source.size != targetRect.dimensions) {
                throw std::runtime_error("Invalid blit source size");
            }
            for (int x = 0; x < targetRect.dimensions.x; x++) {
                for (int y = 0; y < targetRect.dimensions.y; y++) {
                    setPixel(x + targetRect.position.x, y + targetRect.position.y, source.getPixel(x, y));
                }
            }
        }

        Image<T> slice(const Recti &rect) const {
            Image<T> ret = Image<T>(rect.dimensions.x, rect.dimensions.y);
            for (int x = rect.position.x; x < rect.position.x + rect.dimensions.x; x++) {
                for (int y = rect.position.y; y < rect.position.y + rect.dimensions.y; y++) {
                    ret.setPixel(x - rect.position.x, y - rect.position.y, getPixel(x, y));
                }
            }
            return std::move(ret);
        }

        Image<T> swapRows() {
            Image<T> ret = Image<T>(size.x, size.y);
            for (int y = 0; y < size.y; y++) {
                for (int x = 0; x < size.x; x++) {
                    ret.setPixel(size.x - 1 - x, y, getPixel(x, y));
                }
            }
            return std::move(ret);
        }

        Image<T> swapColumns() {
            Image<T> ret = Image<T>(size.x, size.y);
            for (int x = 0; x < size.x; x++) {
                for (int y = 0; y < size.y; y++) {
                    ret.setPixel(x, size.y - 1 - y, getPixel(x, y));
                }
            }
            return std::move(ret);
        }

    protected:
        Vec2i size;
        std::vector<T> buffer;
    };

    typedef Image<ColorRGBA> ImageRGBA;
    typedef Image<ColorRGB> ImageRGB;
}

#endif //XENGINE_IMAGE_HPP
