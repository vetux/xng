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

#include <utility>

#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/objects/rendercanvas.hpp"

namespace xng {
    class RenderPaint final : public RenderObject {
    public:
        static Mat4f getTransform(const Rectf &dstRect, const float rotation) {
            return MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                   * MatrixMath::rotate(Vec3f(0, 0, rotation))
                   * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1));
        }

        RenderPaint() = default;

        RenderPaint(RenderObjectHandle<RenderCanvas> canvas,
                    std::shared_ptr<RenderPipelineTransform> transform,
                    std::shared_ptr<RenderPipelineMaterial> material,
                    const RenderPipeline::DrawID drawID,
                    RenderObjectHandle<RenderMesh> mesh)
            : canvas(std::move(canvas)),
              transform(std::move(transform)),
              material(std::move(material)),
              drawID(drawID),
              mesh(std::move(mesh)) {
        }

        RenderPaint(const RenderPaint &) = default;

        [[nodiscard]] RenderObjectHandle<RenderCanvas> getCanvas() const {
            return canvas;
        }

        [[nodiscard]] RenderPipelineTransform &getTransform() const {
            if (transform == nullptr)
                throw std::runtime_error("Uninitialized RenderPaint");
            return *transform;
        }

        [[nodiscard]] RenderPipelineMaterial &getMaterial() const {
            if (material == nullptr)
                throw std::runtime_error("Uninitialized RenderPaint");
            return *material;
        }

        [[nodiscard]] RenderPipeline::DrawID getDrawID() const {
            if (!canvas.isAssigned())
                throw std::runtime_error("Uninitialized RenderPaint");
            return drawID;
        }

        [[nodiscard]] RenderObjectHandle<RenderMesh> getMesh() const {
            if (!mesh.isAssigned())
                throw std::runtime_error("Uninitialized RenderPaint");
            return mesh;
        }

        bool isUploadComplete() override {
            if (!canvas.isAssigned())
                throw std::runtime_error("Uninitialized RenderPaint");
            return canvas->isUploadComplete() &&
                   transform->isUploadComplete() &&
                   material->isUploadComplete() &&
                   mesh->isUploadComplete();
        }

        void flush() override {
            if (!canvas.isAssigned())
                throw std::runtime_error("Uninitialized RenderPaint");
            canvas->flush();
            transform->flush();
            material->flush();
            mesh->flush();
        }

    private:
        RenderObjectHandle<RenderCanvas> canvas{};
        std::shared_ptr<RenderPipelineTransform> transform = nullptr;
        std::shared_ptr<RenderPipelineMaterial> material = nullptr;
        RenderPipeline::DrawID drawID = 0;

        RenderObjectHandle<RenderMesh> mesh{};
    };
}

#endif //XENGINE_RENDERPAINT_HPP
