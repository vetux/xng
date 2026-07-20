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

#ifndef XENGINE_RENDERSHADER_HPP
#define XENGINE_RENDERSHADER_HPP

#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/pipeline/renderpipeline.hpp"

namespace xng {
    /**
     * User shaders are executed through a built-in pass using forward rendering.
     */
    class RenderShader final : public RenderObject {
    public:
        RenderShader() = default;

        RenderShader(std::shared_ptr<RenderPipeline> pipeline, std::shared_ptr<RenderPipelineShader> shader)
            : pipeline(std::move(pipeline)),
              shader(std::move(shader)) {
        }

        [[nodiscard]] std::shared_ptr<RenderPipeline> getPipeline() const {
            if (pipeline == nullptr)
                throw std::runtime_error("Uninitialized RenderShader");
            return pipeline;
        }

        [[nodiscard]] std::shared_ptr<RenderPipelineShader> getShader() const {
            if (shader == nullptr)
                throw std::runtime_error("Uninitialized RenderShader");
            return shader;
        }

        /**
         * @return The parameters the user shading pass should bind when invoking the pipeline.
         */
        [[nodiscard]] const std::unordered_map<std::string, rg::ShaderPrimitive> &getParameters() const {
            return parameters;
        }

        void setParameter(const std::string &name, rg::ShaderPrimitive value) {
            parameters[name] = std::move(value);
        }

    private:
        std::shared_ptr<RenderPipeline> pipeline = nullptr;
        std::shared_ptr<RenderPipelineShader> shader = nullptr;

        // TODO: Design user buffer bindings definition
        std::unordered_map<std::string, rg::ShaderPrimitive> parameters;
    };
}

#endif //XENGINE_RENDERSHADER_HPP
