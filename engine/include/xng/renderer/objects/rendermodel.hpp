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

#include "xng/math/transform.hpp"
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
        RenderModel(BufferStreamer<ShaderTransform::CPU> &transformStream,
                    std::vector<RenderObjectHandle<RenderMesh> > _meshes,
                    RenderObjectHandle<RenderMaterial> _material,
                    const bool receiveShadows,
                    const bool castShadows)
            : RenderObject(OBJECT_MODEL),
              transformStream(transformStream),
              transformHandle(transformStream.create()),
              meshes(std::move(_meshes)),
              material(std::move(_material)),
              receiveShadows(receiveShadows),
              castShadows(castShadows) {
        }

        ~RenderModel() override {
            transformStream.destroy(transformHandle);
        }

        void setTransform(const Transform &transform) const {
            const auto &model = transform.model();
            transformStream.upload(transformHandle, {model});
        }

        [[nodiscard]] BufferStreamer<ShaderTransform::CPU>::Slot getTransformSlot() const {
            return transformHandle;
        }

        [[nodiscard]] const std::vector<RenderObjectHandle<RenderMesh> > &getMeshes() const {
            return meshes;
        }

        [[nodiscard]] const RenderObjectHandle<RenderMaterial> &getMaterial() const {
            return material;
        }

        [[nodiscard]] bool isReceiveShadows() const {
            return receiveShadows;
        }

        [[nodiscard]] bool isCastShadows() const {
            return castShadows;
        }

        bool isUploadComplete() override {
            for (auto &mesh: meshes) {
                if (!mesh->isUploadComplete()) {
                    return false;
                }
            }
            if (material && !material->isUploadComplete()) {
                return false;
            }
            return transformStream.isUploadComplete(transformHandle);
        }

        void flush() override {
            for (auto &mesh: meshes) {
                mesh->flush();
            }
            if (material) {
                material->flush();
            }
            transformStream.flush(transformHandle);
        }

    private:
        BufferStreamer<ShaderTransform::CPU> &transformStream;
        BufferStreamer<ShaderTransform::CPU>::Slot transformHandle;

        std::vector<RenderObjectHandle<RenderMesh> > meshes;
        RenderObjectHandle<RenderMaterial> material;

        bool receiveShadows = false;
        bool castShadows = false;
    };
}

#endif //XENGINE_RENDERMODEL_HPP
