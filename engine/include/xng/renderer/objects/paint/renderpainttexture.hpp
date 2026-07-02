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

#ifndef XENGINE_RENDERPAINTTEXTURE_HPP
#define XENGINE_RENDERPAINTTEXTURE_HPP

#include <utility>

#include "xng/assets/color.hpp"

#include "xng/math/matrixmath.hpp"

#include "xng/renderer/objects/renderpaint.hpp"
#include "xng/renderer/objects/rendertexture.hpp"

namespace xng {
    class RenderPaintTexture final : public RenderPaint {
    public:
        /**
         * Paint texture
         *
         * @param id
         * @param paintStream
         * @param _srcRect
         * @param _dstRect
         * @param _texture
         * @param _samplingProperties
         * @param mix
         * @param alpha_mix
         * @param mixColor
         * @param center
         * @param rotation
         */
        RenderPaintTexture(const Id id,
                           BufferStreamer<ShaderCanvasPaint::CPU> &paintStream,
                           Rectf _srcRect,
                           Rectf _dstRect,
                           RenderObjectHandle<RenderTexture> _texture,
                           const SamplingProperties &_samplingProperties,
                           const float mix = 0,
                           const float alpha_mix = 0,
                           const ColorRGBA &mixColor = {},
                           const Vec2f &center = {},
                           const float rotation = 0)
            : RenderPaint(OBJECT_PAINT_TEXTURE, id),
              paintStream(paintStream),
              slot(paintStream.create()),
              srcRect(std::move(_srcRect)),
              dstRect(std::move(_dstRect)),
              center(center),
              rotation(rotation),
              color(mixColor),
              texture(std::move(_texture)),
              samplingProperties(std::move(_samplingProperties)),
              mix(mix),
              alpha_mix(alpha_mix) {
            const auto colorF = color.divide();
            ShaderCanvasPaint::CPU paint{};
            paint.color = colorF;
            paint.colorMixFactor = mix;
            paint.alphaMixFactor = alpha_mix;
            paint.texture = texture->getShaderData(samplingProperties);

            paintStream.upload(slot, paint);

            modelMatrix = MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                          * MatrixMath::rotate(Vec3f(0, 0, rotation))
                          * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1));
        }

        ~RenderPaintTexture() override {
            paintStream.destroy(slot);
        }

        [[nodiscard]] Type getPaintType() const {
            return type;
        }

        [[nodiscard]] BufferStreamer<ShaderCanvasPaint::CPU>::Slot getSlot() const {
            return slot;
        }

        [[nodiscard]] const Rectf &getSrcRect() const {
            return srcRect;
        }

        [[nodiscard]] const Rectf &getDstRect() const {
            return dstRect;
        }

        [[nodiscard]] const Vec2f &getCenter() const {
            return center;
        }

        [[nodiscard]] float getRotation() const {
            return rotation;
        }

        [[nodiscard]] const ColorRGBA &getColor() const {
            return color;
        }

        [[nodiscard]] RenderObjectHandle<RenderTexture> getTexture() const {
            return texture;
        }

        [[nodiscard]] const SamplingProperties &getSamplingProperties() const {
            return samplingProperties;
        }

        [[nodiscard]] float getMix() const {
            return mix;
        }

        [[nodiscard]] float getAlphaMix() const {
            return alpha_mix;
        }

        bool isUploadComplete() override {
            if (texture && !texture->isUploadComplete()) {
                return false;
            }
            return paintStream.isUploadComplete(slot);
        }

        void flush() override {
            if (texture) {
                texture->flush();
            }
            paintStream.flush(slot);
        }

    private:
        BufferStreamer<ShaderCanvasPaint::CPU> &paintStream;
        BufferStreamer<ShaderCanvasPaint::CPU>::Slot slot;

        Mat4f modelMatrix;

        Rectf srcRect{};
        Rectf dstRect{};

        Vec2f center{};
        float rotation{};

        ColorRGBA color{};

        RenderObjectHandle<RenderTexture> texture{};
        SamplingProperties samplingProperties;
        float mix{};
        float alpha_mix{};
    };
}

#endif //XENGINE_RENDERPAINTTEXTURE_HPP
