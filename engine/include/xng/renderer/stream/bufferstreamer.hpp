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

#ifndef XENGINE_BUFFERSTREAMER_HPP
#define XENGINE_BUFFERSTREAMER_HPP

#include "xng/renderer/stream/streambuffer.hpp"

namespace xng {
    /**
     * The BufferStreamer streams fixed size data (StorageBuffer).
     *
     * @tparam T sizeof(T) is the size of one element in the streamer.
     */
    template<typename T>
    class BufferStreamer {
    public:
        /**
         * The index of the element in the buffer
         */
        typedef unsigned int Slot;

        explicit BufferStreamer(rg::Heap &heap, ChunkStreamer &chunkStreamer)
            : buffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_STORAGE) {
        }

        Slot create() {
            if (!freeSlots.empty()) {
                const auto ret = freeSlots.back();
                freeSlots.pop_back();
                return ret;
            }
            return nextSlot++;
        }

        void destroy(Slot slot) {
            auto it = pendingUploads.find(slot);
            if (it != pendingUploads.end()) {
                buffer.release(it->second.handle);
            }
            pendingUploads.erase(slot);
            freeSlots.push_back(slot);
        }

        void upload(const Slot slot, const T &data) {
            auto it = pendingUploads.find(slot);
            if (it != pendingUploads.end()) {
                buffer.release(it->second.handle);
                pendingUploads.erase(slot);
            }
            const auto handle = buffer.upload(reinterpret_cast<const uint8_t *>(&data),
                          sizeof(T),
                          slot * sizeof(T));
            pendingUploads.insert(std::pair(slot, PendingUpload{handle}));
        }

        bool isUploadComplete(Slot slot) {
            auto it = pendingUploads.find(slot);
            if (it == pendingUploads.end()) return true;
            return buffer.isUploadComplete(it->second.handle);
        }

        void flush(Slot slot) {
            auto it = pendingUploads.find(slot);
            if (it == pendingUploads.end()) return;
            auto &pendingUpload = it->second;
            if (!pendingUpload.flushed) {
                buffer.flush(pendingUpload.handle);
                pendingUpload.flushed = true;
            }
        }

        rg::HeapResource<rg::Buffer> commit(rg::GraphBuilder &ctx) {
            std::unordered_set<Slot> evictedHandles;
            for (auto &pair: pendingUploads) {
                auto &pendingUpload = pair.second;
                if (pendingUpload.flushed || buffer.isUploadComplete(pendingUpload.handle)) {
                    evictedHandles.insert(pair.first);
                }
            }
            for (auto &slot: evictedHandles) {
                buffer.release(pendingUploads.at(slot).handle);
                pendingUploads.erase(slot);
            }
            return buffer.commit(ctx);
        }

    private:
        struct PendingUpload {
            StreamBuffer::Handle handle;
            bool flushed = false;
        };

        StreamBuffer buffer;

        Slot nextSlot = 0;
        std::vector<Slot> freeSlots;

        std::unordered_map<Slot, PendingUpload> pendingUploads;
    };
}

#endif //XENGINE_BUFFERSTREAMER_HPP
