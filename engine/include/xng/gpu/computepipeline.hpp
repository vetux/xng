/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_COMPUTEPIPELINE_HPP
#define XENGINE_COMPUTEPIPELINE_HPP

#include <utility>
#include <variant>

#include "xng/gpu/renderobject.hpp"
#include "xng/gpu/computepipelinedesc.hpp"
#include "xng/gpu/command.hpp"
#include "xng/gpu/texturebuffer.hpp"
#include "xng/gpu/shaderuniformbuffer.hpp"

#include "xng/math/vector3.hpp"

namespace xng {
    class XENGINE_EXPORT ComputePipeline : public RenderObject {
    public:
        Type getType() override {
            return RENDER_OBJECT_COMPUTE_PIPELINE;
        }

        Command bind() {
            return {Command::COMPUTE_BIND_PIPELINE, ComputePipelineBind(this)};
        }

        /**
         * Set the resources accessible to subsequent execute calls.
         *
         * @param resources
         * @return
         */
        Command setBindings(std::vector<ShaderResource> resources) {
            return {Command::COMPUTE_BIND_DATA, ComputePipelineBindData(std::move(resources))};
        }

        /**
         * @param num_groups The number of work groups in each dimension, cannot be zero.
         * @return
         */
        Command execute(const Vector3<unsigned int> &num_groups) {
            return {Command::COMPUTE_EXECUTE, ComputePipelineExecute(num_groups)};
        }

        virtual const ComputePipelineDesc &getDescription() = 0;
    };
}

#endif //XENGINE_COMPUTEPIPELINE_HPP
