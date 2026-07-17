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

#include "xng/renderer/stream/genericbufferstreamer.hpp"

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
            : streamer(heap, chunkStreamer, sizeof(T)) {
        }

        Slot create() {
            return streamer.create();
        }

        void destroy(const Slot slot) {
            return streamer.destroy(slot);
        }

        void upload(const Slot slot, const T &data) {
            return streamer.upload(slot, reinterpret_cast<const uint8_t *>(&data), sizeof(T));
        }

        bool isUploadComplete(const Slot slot) {
            return streamer.isUploadComplete(slot);
        }

        void flush(const Slot slot) {
            return streamer.flush(slot);
        }

        std::vector<rg::TransferPass> commit(rg::GraphBuilder &graph) {
            return streamer.commit(graph);
        }

        rg::HeapResource<rg::Buffer> getBuffer() const {
            return streamer.getBuffer();
        }

    private:
        GenericBufferStreamer streamer;
    };
}

#endif //XENGINE_BUFFERSTREAMER_HPP
