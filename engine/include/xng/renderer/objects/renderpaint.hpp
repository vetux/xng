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

namespace xng {
    class RenderPaint final : public RenderObject {
    public:
        static Mat4f getTransform(const Rectf &dstRect, const float rotation) {
            return MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                   * MatrixMath::rotate(Vec3f(0, 0, rotation))
                   * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1));
        }

        RenderPaint(const Id id,
                    BufferStreamer<ShaderPaint::CPU> &paintStream,
                    RenderObjectHandle<RenderTransform> _transform,
                    RenderObjectHandle<RenderMesh> _mesh,
                    const ColorRGBA &color,
                    const RenderObjectHandle<RenderTexture> &texture,
                    const SamplingProperties &samplingProperties,
                    const Vec4f &mix,
                    const Rectf &srcRect)
            : RenderObject(OBJECT_PAINT, id),
              paintStream(paintStream),
              paintSlot(paintStream.create()),
              transform(std::move(_transform)),
              mesh(std::move(_mesh)) {
            ShaderPaint::CPU data;
            data.transformIndex = transform->getSlot();
            data.color = color.divide();
            data.texture = texture->getShaderData(samplingProperties);
            data.mix = mix;
            data.srcRect = Vec4f(srcRect.position.x, srcRect.position.y, srcRect.dimensions.x, srcRect.dimensions.y);
            paintStream.upload(paintSlot, data);
        }

        [[nodiscard]] BufferStreamer<ShaderPaint::CPU>::Slot getSlot() const {
            return paintSlot;
        }

        [[nodiscard]] RenderObjectHandle<RenderTransform> getTransform() const {
            return transform;
        }

        [[nodiscard]] RenderObjectHandle<RenderMesh> getMesh() const {
            return mesh;
        }

        bool isUploadComplete() override {
            return paintStream.isUploadComplete(paintSlot)
                   && transform->isUploadComplete()
                   && mesh->isUploadComplete();
        }

        void flush() override {
            paintStream.flush(paintSlot);
            transform->flush();
            mesh->flush();
        }

    private:
        BufferStreamer<ShaderPaint::CPU> &paintStream;
        BufferStreamer<ShaderPaint::CPU>::Slot paintSlot;

        RenderObjectHandle<RenderTransform> transform;
        RenderObjectHandle<RenderMesh> mesh;
    };
}

#endif //XENGINE_RENDERPAINT_HPP
