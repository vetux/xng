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

#ifndef XENGINE_RENDERMODEL_HPP
#define XENGINE_RENDERMODEL_HPP

#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/shadertypes.hpp"
#include "xng/renderer/stream/bufferstreamer.hpp"
#include "xng/renderer/objects/rendermaterial.hpp"
#include "xng/renderer/objects/rendermesh.hpp"

namespace xng {
    /**
     * A collection of mesh objects to be drawn with the specified material / transform.
     */
    class RenderModel final : public RenderObject {
    public:
        RenderModel(const Id id,
                    BufferStreamer<ShaderTransform::CPU> &transformStream,
                    BufferStreamer<ShaderDrawProperties::CPU> &drawPropertiesStream,
                    std::vector<RenderObjectHandle<RenderMesh> > _meshes,
                    RenderObjectHandle<RenderMaterial> _material,
                    const bool receiveShadows)
            : RenderObject(id, OBJECT_MODEL),
              transformStream(transformStream),
              drawPropertiesStream(drawPropertiesStream),
              meshes(std::move(_meshes)),
              material(std::move(_material)) {
            transformHandle = transformStream.create();
            drawPropertiesHandle = drawPropertiesStream.create();
            drawPropertiesStream.upload(drawPropertiesHandle,
                                        {
                                            Vec4i(static_cast<int>(material->getMaterialHandle()), receiveShadows, 0, 0)
                                        });
        }

        ~RenderModel() override {
            drawPropertiesStream.destroy(drawPropertiesHandle);
            transformStream.destroy(transformHandle);
        }

        void setTransform(const Transform &transform) const {
            const auto &model = transform.model();
            transformStream.upload(transformHandle, {model});
        }

        [[nodiscard]] BufferStreamer<ShaderDrawProperties::CPU>::Handle getDrawPropertiesHandle() const {
            return drawPropertiesHandle;
        }

        [[nodiscard]] const std::vector<RenderObjectHandle<RenderMesh> > &getMeshes() const {
            return meshes;
        }

        bool isUploadComplete() override {
            for (auto &mesh : meshes) {
                if (!mesh->isUploadComplete()) {
                    return false;
                }
            }
            if (material && !material->isUploadComplete()) {
                return false;
            }
            return transformStream.isUploadComplete(transformHandle) && drawPropertiesStream.isUploadComplete(drawPropertiesHandle);
        }

        void flush() override {
            for (auto &mesh : meshes) {
                mesh->flush();
            }
            if (material) {
                material->flush();
            }
            transformStream.flush(transformHandle);
            drawPropertiesStream.flush(drawPropertiesHandle);
        }

    private:
        BufferStreamer<ShaderTransform::CPU> &transformStream;
        BufferStreamer<ShaderTransform::CPU>::Handle transformHandle;

        BufferStreamer<ShaderDrawProperties::CPU> &drawPropertiesStream;
        BufferStreamer<ShaderDrawProperties::CPU>::Handle drawPropertiesHandle;

        std::vector<RenderObjectHandle<RenderMesh> > meshes;
        RenderObjectHandle<RenderMaterial> material;
    };
}

#endif //XENGINE_RENDERMODEL_HPP
