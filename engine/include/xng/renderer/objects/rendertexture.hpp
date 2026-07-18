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
    ShaderStruct(ShaderTexture,
                 Vec4i, textureBacking_textureID_arrayLayer,
                 Vec4i, textureSize_maxMip,
                 Vec4i, minFilter_magFilter_mipFilter_wrap,
                 Vec4f, srcRect)

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

        RenderTexture(VirtualTextureStreamer &textureStreamer,
                      rg::Heap &heap,
                      const ImageRGBA &image,
                      const WrappingMethod wrapping = WRAP_REPEAT)
            : backing(TEXTURE_BACKING_VIRTUAL_TEXTURE),
              textureStreamer(textureStreamer) {
            size = image.getResolution().convert<int>();
            maxMip = rg::Texture::calculateMipLevels(image.getResolution()) - 1;
            textureHandle = textureStreamer.create(std::make_shared<ImageTileLoader>(image,
                maxMip + 1,
                textureStreamer.getTileSize(),
                textureStreamer.getTileBorder(),
                wrapping,
                heap));
        }

        RenderTexture(VirtualTextureStreamer &textureStreamer,
                      rg::Heap &heap,
                      const ImageRGBA &image,
                      const WrappingMethod wrapping,
                      const unsigned int mipLevels)
            : backing(TEXTURE_BACKING_VIRTUAL_TEXTURE),
              textureStreamer(textureStreamer),
              maxMip(mipLevels - 1) {
            size = image.getResolution().convert<int>();
            textureHandle = textureStreamer.create(std::make_shared<ImageTileLoader>(image,
                mipLevels,
                textureStreamer.getTileSize(),
                textureStreamer.getTileBorder(),
                wrapping,
                heap));
        }

        RenderTexture(VirtualTextureStreamer &textureStreamer,
                      const std::shared_ptr<TileLoader> &tileLoader)
            : backing(TEXTURE_BACKING_VIRTUAL_TEXTURE),
              textureStreamer(textureStreamer) {
            size = tileLoader->getSize().convert<int>();
            maxMip = tileLoader->getMipLevels() - 1;
            textureHandle = textureStreamer.create(tileLoader);
        }

        ~RenderTexture() {
            textureStreamer.destroy(textureHandle);
        }

        [[nodiscard]] VirtualTextureStreamer::TextureID getHandle() const {
            return textureHandle;
        }

        bool isUploadComplete() {
            return textureStreamer.isUploadComplete(textureHandle);
        }

        Vec2i getSize() const {
            return size;
        }

        unsigned int getMaxMip() const {
            return maxMip;
        }

        [[nodiscard]] TextureBacking getBacking() const {
            return backing;
        }

        [[nodiscard]] ShaderTexture::CPU getShaderData(const SamplingProperties &properties) const {
            ShaderTexture::CPU ret;
            ret.textureBacking_textureID_arrayLayer = Vec4i(backing,
                                                            static_cast<int>(textureHandle),
                                                            0,
                                                            0);
            ret.textureSize_maxMip = Vec4i(size.x,
                                           size.y,
                                           static_cast<int>(maxMip),
                                           0);
            ret.minFilter_magFilter_mipFilter_wrap = Vec4i(properties.minFilter,
                                                           properties.magFilter,
                                                           properties.mipFilter,
                                                           properties.wrapping);
            ret.srcRect = Vec4f(properties.srcRect.position.x,
                                properties.srcRect.position.y,
                                properties.srcRect.dimensions.x,
                                properties.srcRect.dimensions.y);
            return ret;
        }

    private:
        TextureBacking backing{};

        VirtualTextureStreamer &textureStreamer;
        VirtualTextureStreamer::TextureID textureHandle{};

        Vec2i size;
        unsigned int maxMip;
    };
}

#endif //XENGINE_RENDERTEXTURE_HPP
