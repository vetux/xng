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

#ifndef XENGINE_MIPGENERATOR_HPP
#define XENGINE_MIPGENERATOR_HPP

#include <unordered_map>
#include <cstring>

#include "xng/assets/image.hpp"

#include "xng/rendergraph/runtime.hpp"
#include "xng/rendergraph/builder/transferpassbuilder.hpp"

namespace xng {
    class MipGenerator {
    public:
        explicit MipGenerator(rg::Heap &heap)
            : heap(heap) {
        }

        /**
         * Generate the mip images for the given image using hardware blit.
         *
         * @param image The mip level 0 image.
         * @param mipLevels
         * @return The mip images for level 1 to level max.
         */
        [[nodiscard]] std::unordered_map<unsigned int, ImageRGBA> generate(const ImageRGBA &image,
                                                                  const unsigned int mipLevels) const {
            const auto texture = heap.allocateTexture(rg::Texture(rg::Texture::CAPABILITY_TRANSFER_SRC
                                                                  | rg::Texture::CAPABILITY_TRANSFER_DST,
                                                                  image.getResolution()));
            const auto buffer = heap.allocateBuffer(rg::Buffer(image.getBuffer().size() * sizeof(ColorRGBA),
                                                               rg::Buffer::CAPABILITY_TRANSFER_SRC,
                                                               rg::Buffer::MEMORY_CPU_TO_GPU));

            {
                const auto mapping = heap.map(buffer);
                assert(mapping->size() == image.getBuffer().size() * sizeof(ColorRGBA));
                std::memcpy(mapping->data(), image.getBuffer().data(), image.getBuffer().size() * sizeof(ColorRGBA));
            }

            std::vector<rg::TransferPass> passes;

            passes.emplace_back(
                rg::TransferPassBuilder("MipGenerator::Upload")
                .read(buffer)
                .write(texture)
                .execute([buffer, texture](rg::TransferContext &ctx) {
                    ctx.copyBufferToTexture(texture,
                                            buffer,
                                            {},
                                            0,
                                            Rectu({}, texture.getDescription().size),
                                            rg::RGBA8);
                })
            );

            std::unordered_map<unsigned int, std::pair<rg::HeapResource<rg::Texture>, rg::HeapResource<rg::Buffer> > > blits;
            for (int mip = 1; mip < mipLevels; mip++) {
                const auto mipSize = rg::Texture::getMipLevelSize(texture.getDescription().size, mip);
                const auto mipTexture = heap.allocateTexture(rg::Texture(rg::Texture::CAPABILITY_TRANSFER_DST,
                                                                         mipSize));
                const auto mipBuffer = heap.allocateBuffer(rg::Buffer(sizeof(ColorRGBA) * (mipSize.x * mipSize.y),
                                                                      rg::Buffer::CAPABILITY_TRANSFER_DST,
                                                                      rg::Buffer::MEMORY_GPU_TO_CPU));

                passes.emplace_back(rg::TransferPassBuilder("MipGenerator::Blit")
                    .read(texture)
                    .write(mipTexture)
                    .execute([texture, mipTexture](rg::TransferContext &ctx) {
                        ctx.blitTexture(texture,
                                        mipTexture,
                                        rg::Texture::SubResource(),
                                        rg::Texture::SubResource(),
                                        Rectu({}, texture.getDescription().size),
                                        Rectu({}, mipTexture.getDescription().size),
                                        rg::LINEAR);
                    }));
                passes.emplace_back(rg::TransferPassBuilder("MipGenerator::Download")
                    .read(mipTexture)
                    .write(mipBuffer)
                    .execute([mipTexture, mipBuffer](rg::TransferContext &ctx) {
                        ctx.copyTextureToBuffer(mipBuffer,
                                                mipTexture,
                                                {},
                                                0,
                                                Rectu({}, mipTexture.getDescription().size),
                                                rg::RGBA8);
                    }));
                blits[mip] = {mipTexture, mipBuffer};
            }

            auto sem = heap.transfer(passes);
            if (!sem->wait(timeOut)) {
                throw std::runtime_error("Failed to generate mip levels");
            }

            std::unordered_map<unsigned int, ImageRGBA> ret;
            for (auto &pair: blits) {
                ImageRGBA mipImage(pair.second.first.getDescription().size);
                {
                    const auto mipMapping = heap.map(pair.second.second);
                    assert(mipMapping->size() == mipImage.getBuffer().size() * sizeof(ColorRGBA));
                    std::memcpy(mipImage.getBuffer().data(),
                                mipMapping->data(),
                                mipImage.getBuffer().size() * sizeof(ColorRGBA));
                }
                ret[pair.first] = std::move(mipImage);
            }
            return ret;
        }

    private:
        static constexpr size_t timeOut = 10'000'000'000ULL;

        rg::Heap &heap;
    };
}

#endif //XENGINE_MIPGENERATOR_HPP
