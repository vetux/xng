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

#ifndef XENGINE_RENDERCAMERA_HPP
#define XENGINE_RENDERCAMERA_HPP

#include "xng/renderer/camera.hpp"
#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/stream/bufferstreamer.hpp"
#include "xng/renderer/shadertypes.hpp"

namespace xng {
    class RenderCamera final : public RenderObject {
    public:
        RenderCamera(const Id id, BufferStreamer<ShaderCamera::CPU> &cameraStream)
            : RenderObject(id, OBJECT_CAMERA), cameraStream(cameraStream) {
            cameraHandle = cameraStream.create();
        }

        ~RenderCamera() override {
            cameraStream.destroy(cameraHandle);
        }

        void setPerspective(const Transform &cameraTransform,
                            const float nearClip = 0.1f,
                            const float farClip = 1000.0f,
                            const float fov = 90,
                            const float aspectRatio = 4.0f / 3.0f) const {
            const Mat4f projection = Camera::getPerspectiveProjection(fov, aspectRatio, nearClip, farClip);
            const Mat4f view = Camera::getView(cameraTransform);
            cameraStream.upload(cameraHandle, {
                                    Vec4f(cameraTransform.getPosition().x,
                                          cameraTransform.getPosition().y,
                                          cameraTransform.getPosition().z,
                                          0),
                                    view,
                                    projection
                                });
        }

        void setOrthographic(const Transform &cameraTransform,
                             const float left = -1.0f,
                             const float right = 1.0f,
                             const float bottom = -1.0f,
                             const float top = 1.0f,
                             const float nearClip = 0.1f,
                             const float farClip = 1000.0f) const {
            const Mat4f projection = Camera::getOrthographicProjection(left, right, bottom, top, nearClip, farClip);
            const Mat4f view = Camera::getView(cameraTransform);
            cameraStream.upload(cameraHandle,
                                {
                                    Vec4f(cameraTransform.getPosition().x,
                                          cameraTransform.getPosition().y,
                                          cameraTransform.getPosition().z,
                                          0),
                                    view,
                                    projection
                                });
        }

        BufferStreamer<ShaderCamera::CPU>::Handle getHandle() const {
            return cameraHandle;
        }

        bool isUploadComplete() override {
            return cameraStream.isUploadComplete(cameraHandle);
        }

        void flush() override {
            cameraStream.flush(cameraHandle);
        }

    private:
        BufferStreamer<ShaderCamera::CPU> &cameraStream;
        BufferStreamer<ShaderCamera::CPU>::Handle cameraHandle;
    };
}

#endif //XENGINE_RENDERCAMERA_HPP
