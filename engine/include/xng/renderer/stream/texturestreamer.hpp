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

#ifndef XENGINE_TEXTURESTREAMER_HPP
#define XENGINE_TEXTURESTREAMER_HPP

#include "xng/assets/image.hpp"
#include "xng/math/vector2.hpp"
#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/resource/texture.hpp"

#include "xng/renderer/textureresolution.hpp"
#include "xng/renderer/stream/streambuffer.hpp"
#include "xng/renderer/stream/streamtexture.hpp"

namespace xng {
    class TextureStreamer {
    public:
        static Vec2i getTextureResolutionLevelSize(const TextureResolution level) {
            switch (level) {
                case RESOLUTION_8x8: return {8, 8};
                case RESOLUTION_16x16: return {16, 16};
                case RESOLUTION_32x32: return {32, 32};
                case RESOLUTION_64x64: return {64, 64};
                case RESOLUTION_128x128: return {128, 128};
                case RESOLUTION_256x256: return {256, 256};
                case RESOLUTION_512x512: return {512, 512};
                case RESOLUTION_1024x1024: return {1024, 1024};
                case RESOLUTION_2048x2048: return {2048, 2048};
                case RESOLUTION_4096x4096: return {4096, 4096};
                case RESOLUTION_8192x8192: return {8192, 8192};
                default:
                    throw std::runtime_error("Invalid TextureResolution level");
            }
        }

        struct Handle {
            unsigned int index{};
            TextureResolution level{};
            Vec2i size; // The original texture size

            bool assigned() const {
                return index != std::numeric_limits<size_t>::max();
            }

            bool operator==(const Handle &other) const {
                return index == other.index && level == other.level && size == other.size;
            }

            Vec2f getScale() const {
                return size.convert<float>() / getTextureResolutionLevelSize(level).convert<float>();
            }
        };

        Handle upload(const ImageRGBA &image);

        void destroy(const Handle &handle);

        bool isUploadComplete(const Handle &handle);

        void flush(const Handle &handle);

        std::unordered_map<TextureResolution, rg::HeapResource<rg::Texture> > commit(rg::GraphBuilder &ctx);

    private:
        struct PendingUpload {
            StreamBuffer::Handle handle;
            size_t offset; // offset into stream buffer
        };

        StreamBuffer streamBuffer;
        std::unordered_map<Handle, PendingUpload> pendingUploads;
        std::unordered_map<TextureResolution, StreamTexture> textures;
    };
}

#endif //XENGINE_TEXTURESTREAMER_HPP
