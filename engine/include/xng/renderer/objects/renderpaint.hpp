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

#ifndef XENGINE_RENDERPAINT_HPP
#define XENGINE_RENDERPAINT_HPP

#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/stream/bufferstreamer.hpp"
#include "xng/shaderscript/macro/shaderstruct.hpp"

namespace xng {
    ShaderStruct(ShaderPaint,
                 Vec4f, color,
                 ShaderTexture, texture,
                 Vec4f, mix,
                 Vec4f, srcRect)

    class RenderPaint final : public RenderObject {
    public:
        RenderPaint(const Id id,
                    BufferStreamer<ShaderPaint::CPU> &paintStream,
                    const ColorRGBA &color,
                    const RenderObjectHandle<RenderTexture> &texture,
                    const SamplingProperties &samplingProperties,
                    const Vec4f &mix,
                    const Rectf &srcRect)
            : RenderObject(OBJECT_PAINT, id),
              paintStream(paintStream),
              paintSlot(paintStream.create()) {
            ShaderPaint::CPU data;
            data.color = color.divide();
            if (texture) {
                data.texture = texture->getShaderData(samplingProperties);
                data.mix = mix;
                data.srcRect = Vec4f(srcRect.position.x,
                                     srcRect.position.y,
                                     srcRect.dimensions.x,
                                     srcRect.dimensions.y);
            } else {
                data.texture.textureSize_textureID_maxMip = Vec4i(-1);
            }
            paintStream.upload(paintSlot, data);
        }

        [[nodiscard]] BufferStreamer<ShaderPaint::CPU>::Slot getSlot() const {
            return paintSlot;
        }

        bool isUploadComplete() override {
            return paintStream.isUploadComplete(paintSlot);
        }

        void flush() override {
            paintStream.flush(paintSlot);
        }

    private:
        BufferStreamer<ShaderPaint::CPU> &paintStream;
        BufferStreamer<ShaderPaint::CPU>::Slot paintSlot;
    };
}

#endif //XENGINE_RENDERPAINT_HPP
