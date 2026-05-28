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

#ifndef XENGINE_STREAMTEXTURE_HPP
#define XENGINE_STREAMTEXTURE_HPP

#include <compare>
#include <cstddef>
#include <format>
#include <utility>

#include "xng/assets/image.hpp"
#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/resource/texture.hpp"

namespace xng {
    /**
     * Fixed resolution slab-based allocation using Array Texture.
     *
     * CPU Buffer -> StreamBuffer -> Texture
     *
     * Texture slots can also be allocated and bound to shaders for writing instead of streaming from cpu. (Shadow Mapping, IBL)
     */
    class StreamTexture {
    public:
        /**
         * The index into the array texture.
         */
        typedef unsigned int Slot;

        explicit StreamTexture(rg::Heap &heap, ChunkStreamer &chunkStreamer, rg::Texture desc)
            : heap(heap),
              chunkStreamer(chunkStreamer),
              buffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_TRANSFER_SRC),
              bufferAllocator() {
            switch (desc.textureType) {
                case rg::TEXTURE_2D:
                    desc.textureType = rg::TEXTURE_2D_ARRAY;
                    break;
                case rg::TEXTURE_2D_MULTISAMPLE:
                    desc.textureType = rg::TEXTURE_2D_MULTISAMPLE_ARRAY;
                    break;
                case rg::TEXTURE_CUBE_MAP:
                    desc.textureType = rg::TEXTURE_CUBE_MAP_ARRAY;
                    break;
                default:
                    throw std::runtime_error("Unsupported texture type for stream texture");
            }
            desc.capabilities = desc.capabilities | rg::Texture::CAPABILITY_TRANSFER_DST;
            texture = heap.allocateTexture(desc);
        }

        ~StreamTexture() = default;

        Slot create() {
            Slot ret;
            if (!freeSlots.empty()) {
                ret = freeSlots.back();
                freeSlots.pop_back();
            } else {
                ret = nextSlot++;
            }
            pendingUploads[ret] = {};
            return ret;
        }

        void destroy(const Slot &slot) {
            for (auto &upload: pendingUploads.at(slot)) {
                buffer.release(upload.bufferHandle);
                bufferAllocator.free(upload.bufferOffset, upload.bufferSize);
            }
            pendingUploads.erase(slot);
            freeSlots.push_back(slot);
        }

        void upload(const Slot &slot,
                    const ImageRGBA &image,
                    const int mipLevel = 0) {
            const auto uploadsCopy = std::move(pendingUploads.at(slot));
            pendingUploads.at(slot).clear();
            for (auto &upload: uploadsCopy) {
                if (upload.mipLevel == mipLevel) {
                    buffer.release(upload.bufferHandle);
                    bufferAllocator.free(upload.bufferOffset, upload.bufferSize);
                } else {
                    pendingUploads.at(slot).emplace_back(upload);
                }
            }

            const auto offset = bufferAllocator.allocate(sizeof(ColorRGBA) * image.getBuffer().size());
            const auto size = sizeof(ColorRGBA) * image.getBuffer().size();

            const auto bufferHandle = buffer.upload(reinterpret_cast<const uint8_t *>(image.getBuffer().data()),
                                                    size,
                                                    offset);

            pendingUploads[slot].emplace_back(PendingUpload(bufferHandle, rg::SRGB8_ALPHA8, offset, size, mipLevel));
        }

        bool isUploadComplete(const Slot &slot, const int mipLevel) const {
            for (auto &upload: pendingUploads.at(slot)) {
                if (upload.mipLevel == mipLevel) {
                    if (!buffer.isUploadComplete(upload.bufferHandle)) return false;
                    break;
                }
            }
            return true;
        }

        void flush(const Slot &handle) {
            for (auto &upload: pendingUploads.at(handle)) {
                buffer.flush(upload.bufferHandle);
                upload.flushed = true;
            }
        }

        std::vector<rg::TransferPass> commit(rg::GraphBuilder &graph) {
            // Resize / copy texture
            if (nextSlot > texture.getDescription().arrayLayers) {
                const auto staleTexture = texture;
                auto desc = staleTexture.getDescription();
                desc.arrayLayers = nextSlot;
                texture = heap.allocateTexture(desc);
                if (staleTexture.getDescription().arrayLayers > 0) {
                    const auto pass = rg::TransferPassBuilder("StreamTexture/Resize")
                            .read(staleTexture, rg::TextureBinding::Range(0,
                                                                          staleTexture.getDescription().mipLevels,
                                                                          0,
                                                                          staleTexture.getDescription().arrayLayers))
                            .write(texture, rg::TextureBinding::Range(0,
                                                                      staleTexture.getDescription().mipLevels,
                                                                      0,
                                                                      staleTexture.getDescription().arrayLayers
                                   ))
                            .execute([this, staleTexture](rg::TransferContext &ctx) {
                                std::vector<rg::TransferContext::TextureCopyRegion> regions;
                                for (auto i = 0; i < staleTexture.getDescription().mipLevels; i++) {
                                    rg::TransferContext::TextureCopyRegion region;
                                    region.src = rg::Texture::SubResource(i, 0, {});
                                    region.dst = rg::Texture::SubResource(i, 0, {});
                                    region.size = staleTexture.getDescription().getMipLevelSize(i);
                                    if (texture.getDescription().textureType == rg::TEXTURE_CUBE_MAP_ARRAY) {
                                        region.depth = staleTexture.getDescription().arrayLayers * 6;
                                    } else {
                                        region.depth = staleTexture.getDescription().arrayLayers;
                                    }
                                    regions.push_back(region);
                                }
                                ctx.copyTexture(texture, staleTexture, regions);
                            });
                    graph.addPass(pass);
                }
            }

            auto ret = buffer.commit(graph);

            const auto stableBuffer = buffer.getBuffer();

            // Copy flushed / Completed uploads
            const auto pendingUploadsCopy = std::move(pendingUploads);
            pendingUploads.clear();
            for (auto &pair: pendingUploadsCopy) {
                pendingUploads[pair.first] = {};
                for (auto &pendingUpload: pair.second) {
                    if (pendingUpload.committed) {
                        // Pass already generated; wait for ChunkStreamer to finish before releasing.
                        if (buffer.isUploadComplete(pendingUpload.bufferHandle)) {
                            buffer.release(pendingUpload.bufferHandle);
                            bufferAllocator.free(pendingUpload.bufferOffset, pendingUpload.bufferSize);
                        } else {
                            pendingUploads[pair.first].emplace_back(pendingUpload);
                        }
                    } else if (pendingUpload.flushed || buffer.isUploadComplete(pendingUpload.bufferHandle)) {
                        // Execute copy from stream buffer to texture
                        const auto slot = pair.first;
                        const auto upload = pendingUpload;

                        const auto pass = rg::TransferPassBuilder("StreamTexture/Commit")
                                .read(stableBuffer, upload.bufferOffset, upload.bufferSize)
                                .write(texture, rg::TextureBinding::Range(upload.mipLevel, 1, pair.first, 1))
                                .execute([this, slot, upload, stableBuffer](rg::TransferContext &ctx) {
                                    const auto mipSize = texture.getDescription().getMipLevelSize(upload.mipLevel);
                                    ctx.copyBufferToTexture(texture,
                                                            stableBuffer,
                                                            rg::Texture::SubResource(upload.mipLevel, slot, {}),
                                                            upload.bufferOffset,
                                                            Recti({}, mipSize),
                                                            upload.bufferFormat);
                                    ctx.generateMipMaps(texture);
                                });
                        graph.addPass(pass);

                        if (buffer.isUploadComplete(pendingUpload.bufferHandle)) {
                            // ChunkStreamer already done; safe to release now.
                            buffer.release(pendingUpload.bufferHandle);
                            bufferAllocator.free(pendingUpload.bufferOffset, pendingUpload.bufferSize);
                        } else {
                            // Flushed but ChunkStreamer hasn't run yet.
                            // Defer release so chunkStreamer.release() doesn't erase the
                            // upload chunks before chunkStreamer.commit() processes them.
                            PendingUpload deferred = pendingUpload;
                            deferred.committed = true;
                            pendingUploads[pair.first].emplace_back(deferred);
                        }
                    } else {
                        pendingUploads[pair.first].emplace_back(pendingUpload);
                    }
                }
            }

            return ret;
        }

        rg::HeapResource<rg::Texture> getTexture() const {
            return texture;
        }

    private:
        struct PendingUpload {
            StreamBuffer::Handle bufferHandle;
            rg::ColorFormat bufferFormat;

            size_t bufferOffset;
            size_t bufferSize;

            int mipLevel;

            bool flushed;
            bool committed;

            PendingUpload(const StreamBuffer::Handle bufferHandle,
                          const rg::ColorFormat bufferFormat,
                          const size_t bufferOffset,
                          const size_t bufferSize,
                          const int mipLevel)
                : bufferHandle(bufferHandle),
                  bufferFormat(bufferFormat),
                  bufferOffset(bufferOffset),
                  bufferSize(bufferSize),
                  mipLevel(mipLevel),
                  flushed(false),
                  committed(false) {
            }
        };

        rg::Heap &heap;
        ChunkStreamer &chunkStreamer;

        StreamBuffer buffer;
        RangeAllocator bufferAllocator;

        rg::HeapResource<rg::Texture> texture;

        std::unordered_map<Slot, std::vector<PendingUpload> > pendingUploads;

        Slot nextSlot = 0;
        std::vector<Slot> freeSlots;
    };
}

#endif //XENGINE_STREAMTEXTURE_HPP
