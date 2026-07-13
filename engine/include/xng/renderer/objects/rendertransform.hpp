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

#ifndef XENGINE_RENDERTRANSFORM_HPP
#define XENGINE_RENDERTRANSFORM_HPP

#include "xng/renderer/renderobject.hpp"

#include "xng/math/transform.hpp"

namespace xng {
    class RenderTransform final : public RenderObject {
    public:
        RenderTransform(const Id id,
                        BufferStreamer<ShaderTransform::CPU> &transformStream)
            : RenderObject(OBJECT_TRANSFORM, id),
              transformStream(transformStream),
              transformSlot(transformStream.create()) {
        }

        void set(const Transform &transform) const {
            set(transform.model());
        }

        void set(const Mat4f &transform) const {
            ShaderTransform::CPU data;
            data.transform = transform;
            transformStream.upload(transformSlot, data);
        }

        [[nodiscard]] BufferStreamer<ShaderTransform::CPU>::Slot getSlot() const {
            return transformSlot;
        }

        bool isUploadComplete() override {
            return transformStream.isUploadComplete(transformSlot);
        }

        void flush() override {
            transformStream.flush(transformSlot);
        }

    private:
        BufferStreamer<ShaderTransform::CPU> &transformStream;
        BufferStreamer<ShaderTransform::CPU>::Slot transformSlot;
    };
}

#endif //XENGINE_RENDERTRANSFORM_HPP
