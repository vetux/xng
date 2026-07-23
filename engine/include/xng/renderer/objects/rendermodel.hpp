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

#include "xng/renderer/objects/rendertexture.hpp"
#include "xng/renderer/objects/rendermesh.hpp"
#include "xng/renderer/renderpath.hpp"
#include "xng/renderer/shadingmodel.hpp"

namespace xng {
    class RenderModel {
    public:
        RenderModel() = default;

        RenderModel(std::shared_ptr<RenderPipelineTransform> transform,
                    RenderObjectHandle<RenderMaterial> material,
                    const RenderPipeline::DrawID drawID,
                    std::vector<RenderObjectHandle<RenderMesh> > meshes)
            : transform(std::move(transform)),
              material(std::move(material)),
              drawID(drawID),
              meshes(std::move(meshes)),
              castShadows(false) {
        }

        RenderModel(std::shared_ptr<RenderPipelineTransform> transform,
                    RenderObjectHandle<RenderMaterial> material,
                    const RenderPipeline::DrawID drawID,
                    std::vector<RenderObjectHandle<RenderMesh> > meshes,
                    std::shared_ptr<RenderPipelineTransform> shadowTransform,
                    const RenderPipeline::DrawID shadowDrawID)
            : transform(std::move(transform)),
              material(std::move(material)),
              drawID(drawID),
              meshes(std::move(meshes)),
              castShadows(true),
              shadowTransform(std::move(shadowTransform)),
              shadowDrawID(shadowDrawID) {
        }

        RenderModel(const RenderModel &) = default;

        RenderPipelineTransform &getTransform() {
            if (transform == nullptr) {
                throw std::runtime_error("Uninitialized RenderModel");
            }
            return *transform;
        }

        RenderMaterial &getMaterial() {
            if (!material.isAssigned()) {
                throw std::runtime_error("Uninitialized RenderModel");
            }
            return material.get();
        }

        const RenderPipelineTransform &getTransform() const {
            if (transform == nullptr) {
                throw std::runtime_error("Uninitialized RenderModel");
            }
            return *transform;
        }

        const RenderMaterial &getMaterial() const {
            if (!material.isAssigned()) {
                throw std::runtime_error("Uninitialized RenderModel");
            }
            return material.get();
        }

        [[nodiscard]] RenderPipeline::DrawID getDrawID() const {
            return drawID;
        }

        [[nodiscard]] const std::vector<RenderObjectHandle<RenderMesh> > &getMeshes() const {
            return meshes;
        }

        [[nodiscard]] bool isCastShadows() const {
            return castShadows;
        }

        [[nodiscard]] RenderPipelineTransform &getShadowTransform() {
            if (shadowTransform == nullptr) {
                throw std::runtime_error("Uninitialized RenderModel");
            }
            return *shadowTransform;
        }

        [[nodiscard]] RenderPipeline::DrawID getShadowDrawID() const {
            return shadowDrawID;
        }

        void setTransform(const Transform &t) {
            if (transform == nullptr) {
                throw std::runtime_error("Uninitialized RenderModel");
            }

            transform->setTransform(t);

            if (shadowTransform) {
                shadowTransform->setTransform(t);
            }
        }

        bool isUploadComplete() {
            if (transform == nullptr) {
                throw std::runtime_error("Uninitialized RenderModel");
            }
            for (auto &mesh : meshes) {
                if (!mesh->isUploadComplete()) {
                    return false;
                }
            }
            if (shadowTransform) {
                if (!shadowTransform->isUploadComplete()) {
                    return false;
                }
            }
            return transform->isUploadComplete() && material->isUploadComplete();
        }

        void flush() {
            transform->flush();
            material->flush();
            for (auto &mesh : meshes) {
                mesh->flush();
            }
            if (shadowTransform) {
                shadowTransform->flush();
            }
        }

    private:
        std::shared_ptr<RenderPipelineTransform> transform = nullptr;
        RenderObjectHandle<RenderMaterial> material{};

        RenderPipeline::DrawID drawID = 0;

        std::vector<RenderObjectHandle<RenderMesh> > meshes;

        bool castShadows;
        std::shared_ptr<RenderPipelineTransform> shadowTransform = nullptr;
        RenderPipeline::DrawID shadowDrawID = 0;
    };
}

#endif //XENGINE_RENDERMODEL_HPP
