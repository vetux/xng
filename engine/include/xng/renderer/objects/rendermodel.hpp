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
#include "xng/renderer/shadingmodel.hpp"
#include "xng/renderer/renderpath.hpp"
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
                    BufferStreamer<ShaderMesh::CPU> &shaderMeshStream,
                    std::vector<RenderObjectHandle<RenderMesh> > _meshes,
                    RenderObjectHandle<RenderMaterial> _material,
                    const RenderPath renderPath,
                    const ShadingModel shadingModel,
                    const bool receiveShadows,
                    const bool castShadows)
            : RenderObject(OBJECT_MODEL, id),
              transformStream(transformStream),
              transformHandle(transformStream.create()),
              shaderMeshStream(shaderMeshStream),
              meshes(std::move(_meshes)),
              material(std::move(_material)),
              renderPath(renderPath),
              shadingModel(shadingModel),
              receiveShadows(receiveShadows),
              castShadows(castShadows) {
            for (const auto &mesh: meshes) {
                ShaderMesh::CPU shaderMesh{};
                shaderMesh.baseVertex = mesh->getAllocation().baseVertex;
                shaderMesh.indexOffset = mesh->getAllocation().drawCall.offset / sizeof(unsigned int);
                shaderMesh.indexCount = mesh->getAllocation().drawCall.count;
                shaderMesh.modelID = id;
                shaderMesh.meshID = mesh->getId();
                shaderMesh.transformIndex = transformHandle;
                shaderMesh.materialIndex = material->getSlot();
                shaderMesh.receiveShadows = receiveShadows;
                const auto handle = shaderMeshStream.create();
                shaderMeshStream.upload(handle, shaderMesh);
                shaderMeshSlots.emplace_back(handle);
            }
        }

        ~RenderModel() override {
            transformStream.destroy(transformHandle);
            for (auto &handle: shaderMeshSlots) {
                shaderMeshStream.destroy(handle);
            }
        }

        void setTransform(const Transform &transform) const {
            const auto &model = transform.model();
            transformStream.upload(transformHandle, {model});
        }

        [[nodiscard]] BufferStreamer<ShaderTransform::CPU>::Slot getTransformSlot() const {
            return transformHandle;
        }

        [[nodiscard]] const std::vector<BufferStreamer<ShaderMesh::CPU>::Slot> &getShaderMeshSlots() const {
            return shaderMeshSlots;
        }

        [[nodiscard]] const std::vector<RenderObjectHandle<RenderMesh> > &getMeshes() const {
            return meshes;
        }

        [[nodiscard]] const RenderObjectHandle<RenderMaterial> &getMaterial() const {
            return material;
        }

        [[nodiscard]] RenderPath getRenderPath() const {
            return renderPath;
        }

        [[nodiscard]] ShadingModel getShadingModel() const {
            return shadingModel;
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

        BufferStreamer<ShaderMesh::CPU> shaderMeshStream;
        std::vector<BufferStreamer<ShaderMesh::CPU>::Slot> shaderMeshSlots;

        std::vector<RenderObjectHandle<RenderMesh> > meshes;
        RenderObjectHandle<RenderMaterial> material;

        RenderPath renderPath = RENDER_PATH_DEFERRED;
        ShadingModel shadingModel = SHADING_MODEL_UNLIT;

        bool receiveShadows = false;
        bool castShadows = false;
    };
}

#endif //XENGINE_RENDERMODEL_HPP
