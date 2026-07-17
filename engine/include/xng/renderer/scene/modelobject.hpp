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

#ifndef XENGINE_MODELOBJECT_HPP
#define XENGINE_MODELOBJECT_HPP

namespace xng {
    class ModelObject {
    public:
        ModelObject(RenderObjectHandle<RenderTransform> transform,
              RenderObjectHandle<RenderMaterial> material,
              std::vector<RenderObjectHandle<RenderMesh> > meshes,
              const RenderPath renderPath,
              const bool castShadows,
              const ShadingModel shadingModel,
              const bool receiveShadows)
            : transform(std::move(transform)),
              material(std::move(material)),
              meshes(std::move(meshes)),
              renderPath(renderPath),
              castShadows(castShadows),
              shadingModel(shadingModel),
              receiveShadows(receiveShadows) {
        }

        [[nodiscard]] RenderObjectHandle<RenderTransform> getTransform() const {
            return transform;
        }

        [[nodiscard]] RenderObjectHandle<RenderMaterial> getMaterial() const {
            return material;
        }

        [[nodiscard]] const std::vector<RenderObjectHandle<RenderMesh> > &getMeshes() const {
            return meshes;
        }

        [[nodiscard]] RenderPath getRenderPath() const {
            return renderPath;
        }

        [[nodiscard]] ShadingModel getShadingModel() const {
            return shadingModel;
        }

        [[nodiscard]] bool isCastShadows() const {
            return castShadows;
        }

        [[nodiscard]] bool isReceiveShadows() const {
            return receiveShadows;
        }

    private:
        RenderObjectHandle<RenderTransform> transform;
        RenderObjectHandle<RenderMaterial> material;
        std::vector<RenderObjectHandle<RenderMesh> > meshes;

        RenderPath renderPath = RENDER_PATH_DEFERRED;
        bool castShadows = false;

        ShadingModel shadingModel = SHADING_MODEL_NONE;
        bool receiveShadows = false;
    };
}

#endif //XENGINE_MODELOBJECT_HPP
