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

#include <utility>

#include "xng/render/graph2/shader/fgshaderbuilder.hpp"

namespace xng {
    void FGShaderBuilder::defineFunction(const std::string &name,
                                         const std::vector<std::unique_ptr<FGShaderNode> > &body,
                                         const std::unordered_map<std::string, FGShaderValue> &arguments,
                                         const FGShaderValue returnType) {
        std::vector<std::unique_ptr<FGShaderNode> > bodyCopy;
        bodyCopy.reserve(body.size());
        for (auto &node: body) {
            bodyCopy.push_back(node->copy());
        }
        functions.emplace(name, FGShaderFunction(name, arguments, std::move(bodyCopy), returnType));
    }

    FGShaderSource FGShaderBuilder::build(FGShaderSource::ShaderStage stage, const FGAttributeLayout &inputLayout,
                                          const FGAttributeLayout &outputLayout,
                                          const std::unordered_map<std::string, FGShaderValue> &parameters,
                                          const std::unordered_map<std::string, FGShaderBuffer> &buffers,
                                          const std::unordered_map<std::string, FGTexture> &textures,
                                          const std::vector<std::unique_ptr<FGShaderNode> > &mainFunction) {
        std::vector<std::unique_ptr<FGShaderNode> > mainFunctionCopy;
        mainFunctionCopy.reserve(mainFunction.size());
        for (const auto &node: mainFunction) {
            mainFunctionCopy.push_back(node->copy());
        }
        return {
            stage,
            inputLayout,
            outputLayout,
            parameters,
            buffers,
            textures,
            std::move(mainFunctionCopy),
            functions
        };
    }
}
