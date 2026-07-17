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

#ifndef XENGINE_GENERICBUFFERSTREAMER_HPP
#define XENGINE_GENERICBUFFERSTREAMER_HPP


#include "xng/renderer/stream/streambuffer.hpp"

namespace xng {
    /**
     * The BufferStreamer streams fixed size data.
     */
    class GenericBufferStreamer {
    public:
        /**
         * The index of the element in the buffer
         */
        typedef unsigned int Slot;

        explicit GenericBufferStreamer(rg::Heap &heap,
                                       ChunkStreamer &chunkStreamer,
                                       const size_t elementSize)
            : elementSize(elementSize),
              buffer(heap, chunkStreamer, rg::Buffer::CAPABILITY_STORAGE) {
        }

        Slot create() {
            if (!freeSlots.empty()) {
                const auto ret = freeSlots.back();
                freeSlots.pop_back();
                return ret;
            }
            return nextSlot++;
        }

        void destroy(const Slot slot) {
            auto it = regions.find(slot);
            if (it != regions.end()) {
                buffer.release(it->second.handle);
            }
            regions.erase(slot);
            freeSlots.push_back(slot);
        }

        void upload(const Slot slot, const uint8_t *data, const size_t size) {
            assert(size == elementSize);

            auto it = regions.find(slot);
            if (it != regions.end()) {
                buffer.release(it->second.handle);
                regions.erase(slot);
            }
            const auto handle = buffer.upload(data,
                                              elementSize,
                                              slot * elementSize);
            regions.insert(std::pair(slot, Region{handle}));
        }

        bool isUploadComplete(const Slot slot) {
            auto it = regions.find(slot);
            if (it == regions.end()) return true;
            return buffer.isUploadComplete(it->second.handle);
        }

        void flush(const Slot slot) {
            auto it = regions.find(slot);
            if (it == regions.end()) return;
            auto &pendingUpload = it->second;
            if (!pendingUpload.flushed) {
                buffer.flush(pendingUpload.handle);
                pendingUpload.flushed = true;
            }
        }

        void commit(rg::GraphBuilder &graph) {
            buffer.commit(graph);
        }

        rg::HeapResource<rg::Buffer> getBuffer() const {
            return buffer.getBuffer();
        }

    private:
        struct Region {
            StreamBuffer::Handle handle;
            bool flushed = false;
        };

        const size_t elementSize;

        StreamBuffer buffer;

        Slot nextSlot = 0;
        std::vector<Slot> freeSlots;

        std::unordered_map<Slot, Region> regions;
    };
}

#endif //XENGINE_GENERICBUFFERSTREAMER_HPP
