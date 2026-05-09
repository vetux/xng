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
     *
     * @tparam T sizeof(T) is the size of one element in the streamer.
     */
    template<typename T>
    class BufferStreamer {
    public:
        /**
         * The index of the element in the buffer
         */
        typedef unsigned int Handle;

        BufferStreamer(const rg::Heap &heap);

        Handle create();

        void upload(Handle handle, const T &data) const;

        void destroy(Handle handle);

        bool isUploadComplete(Handle handle);

        void flush(Handle handle);

        rg::HeapResource<rg::Buffer> commit(rg::GraphBuilder &ctx);

    private:
        StreamBuffer buffer;

        //Allocation Range Tracking
    };
}

#endif //XENGINE_BUFFERSTREAMER_HPP
