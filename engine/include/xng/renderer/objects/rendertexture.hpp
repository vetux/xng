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

#ifndef XENGINE_RENDERTEXTURE_HPP
#define XENGINE_RENDERTEXTURE_HPP

#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/stream/texturestreamer.hpp"

namespace xng {
    // TODO: Design texture mip level streaming user interface
    class RenderTexture final : public RenderObject {
    public:
        explicit RenderTexture(const Id id, TextureStreamer &textureStreamer, const Vec2i &resolution)
            : RenderObject(OBJECT_TEXTURE, id), textureStreamer(textureStreamer) {
            textureHandle = textureStreamer.create(resolution);
        }

        ~RenderTexture() override {
            textureStreamer.destroy(textureHandle);
        }

        void setImage(const ImageRGBA &image, const int mipLevel = 0) const {
            textureStreamer.upload(textureHandle, image, mipLevel);
        }

        [[nodiscard]] TextureStreamer::Handle getHandle() const {
            return textureHandle;
        }

        bool isUploadComplete() override {
            return textureStreamer.isUploadComplete(textureHandle, 0);
        }

        void flush() override {
            textureStreamer.flush(textureHandle);
        }

    private:
        TextureStreamer &textureStreamer;
        TextureStreamer::Handle textureHandle{};
    };
}

#endif //XENGINE_RENDERTEXTURE_HPP
