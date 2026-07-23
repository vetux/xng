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

#ifndef XENGINE_RENDERMATERIAL_HPP
#define XENGINE_RENDERMATERIAL_HPP

#include "xng/renderer/shadingmodel.hpp"
#include "xng/renderer/renderpath.hpp"

#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/objects/rendertexture.hpp"
#include "xng/renderer/objects/rendershader.hpp"
#include "xng/renderer/pipeline/renderpipeline.hpp"

namespace xng {
    class RenderMaterial final : public RenderObject {
    public:
        RenderMaterial() = default;

        explicit RenderMaterial(std::shared_ptr<RenderPipelineMaterial> materialHandle,
                                const ShadingModel shadingModel,
                                const RenderPath renderPath)
            : materialHandle(std::move(materialHandle)),
              shadingModel(shadingModel),
              renderPath(renderPath) {
        }

        explicit RenderMaterial(std::shared_ptr<RenderPipelineMaterial> materialHandle,
                                RenderObjectHandle<RenderShader> shader)
            : materialHandle(std::move(materialHandle)),
              shader(std::move(shader)) {
        }

        RenderMaterial(const RenderMaterial &) = default;

        void set(const std::unordered_map<RenderPipelineMaterial::PropertyID, rg::ShaderPrimitive> &properties,
                 const std::unordered_map<RenderPipelineMaterial::TextureID,
                     RenderPipelineMaterial::TextureSampler> &textures) {
            if (materialHandle == nullptr)
                throw std::runtime_error("Uninitialized RenderMaterial");
            materialHandle->update(properties, textures);
        }

        std::shared_ptr<RenderPipelineMaterial> getHandle() const {
            if (materialHandle == nullptr)
                throw std::runtime_error("Uninitialized RenderMaterial");
            return materialHandle;
        }

        [[nodiscard]] const RenderObjectHandle<RenderShader> &getShader() const {
            return shader;
        }

        [[nodiscard]] ShadingModel getShadingModel() const {
            return shadingModel;
        }

        [[nodiscard]] RenderPath getRenderPath() const {
            return renderPath;
        }

        bool isUploadComplete() override {
            if (materialHandle == nullptr)
                throw std::runtime_error("Uninitialized RenderMaterial");
            return materialHandle->isUploadComplete();
        }

        void flush() override {
            if (materialHandle == nullptr)
                throw std::runtime_error("Uninitialized RenderMaterial");
            materialHandle->flush();
        }

    private:
        std::shared_ptr<RenderPipelineMaterial> materialHandle = nullptr;

        RenderObjectHandle<RenderShader> shader{};

        ShadingModel shadingModel = SHADING_MODEL_NONE;
        RenderPath renderPath = RENDER_PATH_FORWARD;
    };
}

#endif //XENGINE_RENDERMATERIAL_HPP
