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

namespace xng {
    class RenderTexture final : public RenderObject {
    public:
        //TODO: Multiple backing techniques for textures
        RenderTexture(const Id id,
                      VirtualTextureStreamer &textureStreamer,
                      rg::Heap &heap,
                      const ImageRGBA &image,
                      const WrappingMethod wrapping = WRAP_REPEAT)
            : RenderObject(OBJECT_TEXTURE, id), textureStreamer(textureStreamer) {
            size = image.getResolution().convert<int>();
            maxMip = rg::Texture::calculateMipLevels(image.getResolution()) - 1;
            textureHandle = textureStreamer.create(std::make_shared<ImageTileLoader>(image,
                maxMip + 1,
                textureStreamer.getTileSize(),
                textureStreamer.getTileBorder(),
                wrapping,
                heap));
        }

        RenderTexture(const Id id,
                      VirtualTextureStreamer &textureStreamer,
                      const std::shared_ptr<TileLoader> &tileLoader)
            : RenderObject(OBJECT_TEXTURE, id), textureStreamer(textureStreamer) {
            size = tileLoader->getSize().convert<int>();
            maxMip = tileLoader->getMipLevels() - 1;
            textureHandle = textureStreamer.create(tileLoader);
        }

        ~RenderTexture() override {
            textureStreamer.destroy(textureHandle);
        }

        [[nodiscard]] VirtualTextureStreamer::TextureID getHandle() const {
            return textureHandle;
        }

        bool isUploadComplete() override {
            return textureStreamer.isUploadComplete(textureHandle);
        }

        void flush() override {
        }

        Vec2i getSize() const {
            return size;
        }

        unsigned int getMaxMip() const {
            return maxMip;
        }

        [[nodiscard]] ShaderTexture::CPU getShaderData(const SamplingProperties &properties) const {
            ShaderTexture::CPU ret;
            ret.textureSize_textureID_maxMip = Vec4i(size.x,
                                                     size.y,
                                                     static_cast<int>(textureHandle),
                                                     static_cast<int>(maxMip));
            ret.minFilter_magFilter_mipFilter_wrap = Vec4i(properties.minFilter,
                                                           properties.magFilter,
                                                           properties.mipFilter,
                                                           properties.wrapping);
            return ret;
        }

    private:
        VirtualTextureStreamer &textureStreamer;
        VirtualTextureStreamer::TextureID textureHandle{};

        Vec2i size;
        unsigned int maxMip;
    };
}

#endif //XENGINE_RENDERTEXTURE_HPP
