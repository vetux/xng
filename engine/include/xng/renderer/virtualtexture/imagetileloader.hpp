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

#include "xng/renderer/virtualtexture/tileloader.hpp"
#include "xng/renderer/virtualtexture/tilestreamer.hpp"
#include "xng/renderer/mipgenerator.hpp"

#include "xng/assets/image.hpp"

namespace xng {
    /**
     * For the given image the image tile loader will generate the mips, split them into tiles and generate correct tile borders.
     *
     * In Production the editor will precook the tiles and load from disk with a separate TileLoader implementation.
     */
    class ImageTileLoader final : public TileLoader {
    public:
        struct TiledImage {
            Vec2u tileCount;
            std::vector<std::vector<uint8_t> > tiles;

            TiledImage() = default;

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
            const auto &imageRes = image.getResolution();
            TiledImage ret(TileStreamer::getTiles(imageRes, tileSize));
            std::vector<std::shared_ptr<Task> > tasks;
            const auto atlasTileSize = tileSize + tileBorder * 2;
            for (auto tileX = 0u; tileX < ret.tileCount.x; tileX++) {
                for (auto tileY = 0u; tileY < ret.tileCount.y; tileY++) {
                    auto task = ThreadPool::getPool().addTask(
                        [&image,
                            &imageRes,
                            &ret,
                            tileX,
                            tileY,
                            tileSize,
                            tileBorder,
                            atlasTileSize,
                            wrapping]() {
                            const auto tilePos = Vec2u(tileX * tileSize, tileY * tileSize);
                            auto tileDim = Vec2u(tileSize, tileSize);
                            if (tilePos.x + tileDim.x > imageRes.x) {
                                tileDim.x = imageRes.x - tilePos.x;
                            }
                            if (tilePos.y + tileDim.y > imageRes.y) {
                                tileDim.y = imageRes.y - tilePos.y;
                            }

                            ImageRGBA tile = image.slice(Rectu(tilePos, tileDim));

                            ImageRGBA atlasTile(atlasTileSize, atlasTileSize);
                            atlasTile.blit(Vec2u(tileBorder, tileBorder), tile);

                            // Blit Left Border Edge
                            for (auto x = 0; x < tileBorder; x++) {
                                for (auto y = 0; y < tileDim.y + 2 * tileBorder; y++) {
                                    const auto srcPos = Vec2i(
                                        static_cast<int>(tilePos.x) - (static_cast<int>(tileBorder) - x),
                                        static_cast<int>(tilePos.y) + y - static_cast<int>(tileBorder));
                                    copyTexel(image,
                                              atlasTile,
                                              srcPos,
                                              Vec2u(x, y),
                                              wrapping);
                                }
                            }

                            // Blit Right Border Edge
                            for (auto x = 0; x < tileBorder; x++) {
                                for (auto y = 0; y < tileDim.y + 2 * tileBorder; y++) {
                                    const auto srcPos = Vec2i(static_cast<int>(tilePos.x + tileDim.x) + x,
                                                              static_cast<int>(tilePos.y) + y - static_cast<int>(
                                                                  tileBorder));
                                    copyTexel(image,
                                              atlasTile,
                                              srcPos,
                                              Vec2u((tileBorder + tileDim.x) + x, y),
                                              wrapping);
                                }
                            }

                            // Blit Top Border Edge
                            for (auto y = 0; y < tileBorder; y++) {
                                for (auto x = 0; x < tileDim.x; x++) {
                                    const auto srcPos = Vec2i(static_cast<int>(tilePos.x) + x,
                                                              static_cast<int>(tilePos.y) - (
                                                                  static_cast<int>(tileBorder) - y));
                                    copyTexel(image,
                                              atlasTile,
                                              srcPos,
                                              Vec2u(x + tileBorder, y),
                                              wrapping);
                                }
                            }

                            // Blit Bottom Border Edge
                            for (auto y = 0; y < tileBorder; y++) {
                                for (auto x = 0; x < tileDim.x; x++) {
                                    const auto srcPos = Vec2i(static_cast<int>(tilePos.x) + x,
                                                              static_cast<int>(tilePos.y + tileDim.y) + y);
                                    copyTexel(image,
                                              atlasTile,
                                              srcPos,
                                              Vec2u(x + tileBorder, (tileBorder + tileDim.y) + y),
                                              wrapping);
                                }
                            }

                            auto bytes = std::vector<uint8_t>(atlasTile.getBuffer().size() * sizeof(ColorRGBA));
                            std::memcpy(bytes.data(), atlasTile.getBuffer().data(), bytes.size());

                            ret.setTile({tileX, tileY}, std::move(bytes));
                        });
                    tasks.emplace_back(task);
                }
            }
            for (auto &task: tasks) {
                task->join();
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
            if (size.x > std::numeric_limits<int>::max()
                || size.y > std::numeric_limits<int>::max()) {
                throw std::runtime_error("Image resolution must fit in int");
            }
            const auto mipImages = MipGenerator(heap).generate(image, mipLevels);
            mips.resize(mipLevels);
            std::vector<std::shared_ptr<Task> > tasks;
            tasks.emplace_back(ThreadPool::getPool().addTask([&]() {
                mips.at(0) = generateTiles(image, tileSize, tileBorder, wrapping);
            }));
            for (auto &pair: mipImages) {
                tasks.emplace_back(ThreadPool::getPool().addTask([&]() {
                    mips[pair.first] = generateTiles(pair.second, tileSize, tileBorder, wrapping);
                }));
            }
            for (auto &task: tasks) {
                task->join();
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

        std::vector<uint8_t> getTile(const unsigned int mipLevel, const Vec2u &tile) override {
            return mips.at(mipLevel).getTile(tile);
        }

    private:
        static void copyTexel(const ImageRGBA &source,
                        ImageRGBA &target,
                        const Vec2i &sourcePos,
                        const Vec2u &targetPos,
                        const WrappingMethod wrapping) {
            const Vec2u pos(wrapCoord(sourcePos.x, static_cast<int>(source.getResolution().x), wrapping),
                            wrapCoord(sourcePos.y, static_cast<int>(source.getResolution().y), wrapping));
            target.setPixel(targetPos.x, targetPos.y, source.getPixel(pos.x, pos.y));
        }

        static unsigned int wrapCoord(const int coord, const int resolution, const WrappingMethod wrapping) {
            if (resolution <= 0) {
                return 0;
            }
            if (wrapping == WRAP_REPEAT) {
                // Proper modulo wrap - handles borders many times larger than the resolution,
                // e.g. a 1-2px image with an 8px+ tileBorder.
                int wrapped = coord % resolution;
                if (wrapped < 0) {
                    wrapped += resolution;
                }
                return static_cast<unsigned int>(wrapped);
            }
            // Clamp - safe regardless of how far out of range coord is.
            if (coord < 0) {
                return 0;
            }
            if (coord >= resolution) {
                return static_cast<unsigned int>(resolution - 1);
            }
            return static_cast<unsigned int>(coord);
        }

        Vec2u size;
        unsigned mipLevels;
        WrappingMethod wrapping;
        std::vector<TiledImage> mips;
    };
}

#endif //XENGINE_IMAGETILELOADER_HPP
