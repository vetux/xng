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

#ifndef XENGINE_IMAGETILELOADER_HPP
#define XENGINE_IMAGETILELOADER_HPP

#include "xng/renderer/texturestream/tileloader.hpp"

#include "xng/assets/image.hpp"

#include "xng/renderer/mipgenerator.hpp"

namespace xng {
    /**
     * For the given image the image tile loader will generate the mips, split them into tiles and generate correct tile borders.
     */
    class ImageTileLoader final : public TileLoader {
    public:
        struct TiledImage {
            Vec2u tileCount;
            std::vector<std::vector<uint8_t> > tiles;

            explicit TiledImage(const Vec2u &tileCount)
                : tileCount(tileCount), tiles(tileCount.x * tileCount.y) {
            }

            [[nodiscard]] const std::vector<uint8_t> &getTile(const Vec2u &tile) const {
                const auto index = tile.y * tileCount.x + tile.x;
                return tiles.at(index);
            }

            void setTile(const Vec2u &tile, std::vector<uint8_t> tileData) {
                const auto index = tile.y * tileCount.x + tile.x;
                tiles.at(index) = std::move(tileData);
            }
        };

        static TiledImage generateTiles(const ImageRGBA &image,
                                        const unsigned int tileSize,
                                        const unsigned int tileBorder,
                                        const WrappingMethod wrapping) {
            TiledImage ret(Vec2u(ceildiv(image.getResolution().x, tileSize),
                                 ceildiv(image.getResolution().y, tileSize)));
            const auto atlasTileSize = tileSize + tileBorder * 2;
            for (auto tileX = 0u; tileX < ret.tileCount.x; tileX++) {
                for (auto tileY = 0u; tileY < ret.tileCount.y; tileY++) {
                    const auto tileDim = Vec2u(std::min(tileSize, image.getResolution().x - tileX * tileSize),
                                               std::min(tileSize, image.getResolution().y - tileY * tileSize));
                    ImageRGBA atlasTile(atlasTileSize, atlasTileSize);
                    ImageRGBA tile = image.slice(Rectu({tileX * tileSize, tileY * tileSize},
                                                       tileDim));
                    atlasTile.blit(Vec2u(tileBorder, tileBorder), tile);

                    // Blit borders...

                    auto bytes = std::vector<uint8_t>(atlasTile.getBuffer().size() * sizeof(ColorRGBA));
                    std::memcpy(bytes.data(), atlasTile.getBuffer().data(), bytes.size());
                    ret.setTile({tileX, tileY}, std::move(bytes));
                }
            }
            return ret;
        }

        ImageTileLoader(const ImageRGBA &image,
                        const unsigned int mipLevels,
                        const unsigned int tileSize,
                        const unsigned int tileBorder,
                        const WrappingMethod wrapping,
                        rg::Heap &heap)
            : size(image.getResolution()),
              mipLevels(mipLevels),
              wrapping(wrapping) {
            const auto mipImages = MipGenerator(heap).generate(image, mipLevels);
            for (auto &pair: mipImages) {
                mips.emplace_back(generateTiles(pair.second, tileSize, tileBorder, wrapping));
            }
        }

        ~ImageTileLoader() override = default;

        const Vec2u &getSize() override {
            return size;
        }

        unsigned getMipLevels() override {
            return mipLevels;
        }

        WrappingMethod getWrappingMethod() override {
            return wrapping;
        }

        std::vector<uint8_t> getTile(const int mipLevel, const Vec2u &tile) override {
            return mips.at(mipLevel).getTile(tile);
        }

    private:
        static constexpr unsigned int ceildiv(const unsigned int a, const unsigned int b) {
            return (a + b - 1) / b;
        }

        Vec2u size;
        unsigned mipLevels;
        WrappingMethod wrapping;
        std::vector<TiledImage> mips;
    };
}

#endif //XENGINE_IMAGETILELOADER_HPP
