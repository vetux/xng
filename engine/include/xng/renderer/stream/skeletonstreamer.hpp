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

#ifndef XENGINE_SKELETONSTREAMER_HPP
#define XENGINE_SKELETONSTREAMER_HPP

#include "xng/util/rangeallocator.hpp"
#include "xng/renderer/stream/streambuffer.hpp"

namespace xng {
    /**
     * The skeleton streamer streams sets of bones via a StreamBuffer
     */
    class SkeletonStreamer {
    public:
        /**
         * The index of the first bone in the buffer
         */
        typedef size_t BaseBone;

        SkeletonStreamer(rg::Heap &heap, ChunkStreamer &chunkStreamer)
            : buffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_STORAGE | rg::Buffer::CAPABILITY_TRANSFER_DST) {
        }

        BaseBone create(const size_t count) {
            auto ret = allocator.allocate(count);
            allocations.insert({ret, count});
            return ret;
        }

        void destroy(const BaseBone base) {
            const auto it = pendingUploads.find(base);
            if (it != pendingUploads.end()) {
                buffer.release(it->second);
            }
            allocator.free(base, allocations.at(base));
            allocations.erase(base);
            pendingUploads.erase(base);
        }

        void upload(const BaseBone base, const std::vector<Mat4f> &matrices) {
            if (allocations.at(base) != matrices.size()) {
                throw std::runtime_error("Bone matrices upload count must match allocation");
            }
            const auto it = pendingUploads.find(base);
            if (it != pendingUploads.end()) {
                buffer.release(it->second);
                pendingUploads.erase(it);
            }
            const auto bufferHandle = buffer.upload(reinterpret_cast<const uint8_t *>(matrices.data()),
                                                    matrices.size() * sizeof(Mat4f),
                                                    base * sizeof(Mat4f));
            pendingUploads.insert({base, bufferHandle});
        }

        bool isUploadComplete(const BaseBone base) {
            auto it = pendingUploads.find(base);
            if (it != pendingUploads.end()) {
                return buffer.isUploadComplete(it->second);
            }
            return true;
        }

        void flush(const BaseBone base) {
            const auto it = pendingUploads.find(base);
            if (it != pendingUploads.end()) {
                buffer.flush(it->second);
            }
        }

        std::vector<rg::TransferPass> commit(rg::GraphBuilder &graph) {
            return buffer.commit(graph);
        }

        rg::HeapResource<rg::Buffer> getBuffer() const {
            return buffer.getBuffer();
        }

    private:
        std::unordered_map<BaseBone, size_t> allocations;
        std::unordered_map<BaseBone, StreamBuffer::Handle> pendingUploads;
        RangeAllocator allocator;
        StreamBuffer buffer;
    };
}

#endif //XENGINE_SKELETONSTREAMER_HPP
