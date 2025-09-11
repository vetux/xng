/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_FGSHADERBUILDER_HPP
#define XENGINE_FGSHADERBUILDER_HPP

#include <memory>
#include <utility>
#include <memory>
#include <optional>

#include "fgshaderfunction.hpp"
#include "nodes/fgnodevariablewrite.hpp"
#include "xng/render/graph2/shader/fgshaderliteral.hpp"
#include "xng/render/graph2/shader/fgshadernode.hpp"
#include "xng/render/graph2/shader/fgshadersource.hpp"

namespace xng {
    class XENGINE_EXPORT FGShaderBuilder {
    public:
        void defineFunction(const std::string &name,
                            const std::vector<std::unique_ptr<FGShaderNode> > &body,
                            const std::unordered_map<std::string, FGShaderValue> &arguments,
                            FGShaderValue returnType);

        FGShaderSource build(FGShaderSource::ShaderStage stage,
                             const FGAttributeLayout &inputLayout,
                             const FGAttributeLayout &outputLayout,
                             const std::unordered_map<std::string, FGShaderValue> &parameters,
                             const std::unordered_map<std::string, FGShaderBuffer> &buffers,
                             const std::unordered_map<std::string, FGTexture> &textures,
                             const std::vector<std::unique_ptr<FGShaderNode> > &mainFunction);

    private:
        std::unordered_map<std::string, FGShaderFunction> functions;
    };
}

#endif //XENGINE_FGSHADERBUILDER_HPP
