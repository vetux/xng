/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_RENDERPIPELINE_HPP
#define XENGINE_RENDERPIPELINE_HPP

#include "xng/gpu/renderobject.hpp"
#include "xng/gpu/renderpipelinedesc.hpp"
#include "xng/gpu/texturearraybuffer.hpp"
#include "xng/gpu/shaderstoragebuffer.hpp"

#include "xng/gpu/command.hpp"

namespace xng {
    class XENGINE_EXPORT RenderPipeline : public RenderObject {
    public:
        Type getType() override {
            return RENDER_OBJECT_RENDER_PIPELINE;
        }

        Command bind() {
            return {Command::BIND_PIPELINE, RenderPipelineBind(this)};
        }

        /**
         * @param resources
         * @return The command binds the given resources to a previously bound pipeline
         */
        static Command bindShaderResources(std::vector<ShaderResource> resources) {
            return {Command::BIND_SHADER_RESOURCES, ShaderResourceBind(std::move(resources))};
        }

        virtual std::vector<uint8_t> cache() = 0;

        virtual const RenderPipelineDesc &getDescription() = 0;
    };
}

#endif //XENGINE_RENDERPIPELINE_HPP
