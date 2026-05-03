/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2026 Julia Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_RENDERTEXTURE_HPP
#define XENGINE_RENDERTEXTURE_HPP

#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/stream/texturestreamer.hpp"

namespace xng {
    class RenderTexture final : public RenderObject {
    public:
        explicit RenderTexture(const Id id,
                               TextureStreamer &textureStreamer,
                               const ImageRGBA &image)
            : RenderObject(id, OBJECT_TEXTURE), textureStreamer(textureStreamer) {
            textureHandle = textureStreamer.upload(image);
        }

        ~RenderTexture() override {
            textureStreamer.destroy(textureHandle);
        }

        [[nodiscard]] TextureStreamer::Handle getTextureHandle() const {
            return textureHandle;
        }

        bool isUploadComplete() override {
            return textureStreamer.isUploadComplete(textureHandle);
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
