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
#include "xng/renderer/virtualtexture/imagetileloader.hpp"
#include "xng/renderer/virtualtexture/virtualtexturestreamer.hpp"
#include "xng/shaderscript/macro/shaderstruct.hpp"

namespace xng {
    class RenderTexture final : public RenderObject {
    public:
        //TODO: Multiple backing techniques for textures
        enum TextureBacking : int {
            // Texture stored as virtual texture (High Sampling overhead, No Cross-Mip Anisotropic filtering, Automatic tile level texture streaming, No limits)
            TEXTURE_BACKING_VIRTUAL_TEXTURE = 0,

            // Texture stored in array texture (Low Sampling Overhead, Fixed resolution levels, Array layer limit)
            TEXTURE_BACKING_ARRAY_TEXTURE,

            // Texture stored in a separate texture object (Lowest Sampling Overhead, Binding limits, Limited resolution)
            TEXTURE_BACKING_TEXTURE,
        };

        RenderTexture() = default;

        RenderTexture(VirtualTextureStreamer &textureStreamer,
                      const VirtualTextureStreamer::TextureID textureHandle,
                      Vec2u size,
                      const unsigned int maxMip)
            : backing(TEXTURE_BACKING_VIRTUAL_TEXTURE),
              textureStreamer(&textureStreamer),
              textureHandle(textureHandle),
              size(std::move(size)),
              maxMip(maxMip) {
        }

        [[nodiscard]] VirtualTextureStreamer::TextureID getHandle() const {
            return textureHandle;
        }

        bool isUploadComplete() override {
            if (!textureStreamer) {
                throw std::runtime_error("Uninitialized RenderTexture");
            }
            return textureStreamer->isUploadComplete(textureHandle);
        }

        Vec2u getSize() const {
            return size;
        }

        unsigned int getMaxMip() const {
            return maxMip;
        }

        [[nodiscard]] TextureBacking getBacking() const {
            return backing;
        }

    private:
        TextureBacking backing{};

        VirtualTextureStreamer *textureStreamer = nullptr;
        VirtualTextureStreamer::TextureID textureHandle{};

        Vec2u size;
        unsigned int maxMip;
    };
}

#endif //XENGINE_RENDERTEXTURE_HPP
