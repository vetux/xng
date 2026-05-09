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

#ifndef XENGINE_LOOPBUILDER_HPP
#define XENGINE_LOOPBUILDER_HPP

#include <memory>
#include <vector>

#include "xng/shaderscript/blockscope.hpp"

#include "xng/rendergraph/shader/shaderinstructionfactory.hpp"

namespace xng::ShaderScript
{
    class XENGINE_EXPORT LoopBuilder
    {
    public:
        void BeginFor(rg::ShaderOperand loopVariable, rg::ShaderOperand initializer, rg::ShaderOperand predicate, rg::ShaderOperand iterator)
        {
            if (loopVariable.type != rg::ShaderOperand::Variable) throw std::runtime_error("Invalid loop variable");
            if (initializer.type == rg::ShaderOperand::Variable) throw std::runtime_error("Invalid loop initializer");
            if (iterator.type == rg::ShaderOperand::Variable) throw std::runtime_error("Invalid loop iterator");

            BlockScope::get().addInstruction(rg::ShaderInstructionFactory::assign(loopVariable, initializer));
            _initializer = std::move(initializer);
            _iterator = rg::ShaderOperand::instruction(rg::ShaderInstructionFactory::assign(loopVariable, iterator));
            _predicate = std::move(predicate);
            blockScope = std::make_unique<BlockScope>();
        }

        void EndFor()
        {
            if (blockScope == nullptr) throw std::runtime_error("EndFor without preceding BeginFor");
            body = blockScope->buildInstructionStream();
            blockScope = nullptr;
        }

        rg::ShaderInstruction build() const
        {
            if (blockScope != nullptr) throw std::runtime_error("Incomplete loop block");
            return rg::ShaderInstructionFactory::loop(_initializer, _predicate, _iterator, body);
        }

    private:
        std::unique_ptr<BlockScope> blockScope;

        rg::ShaderOperand _initializer;
        rg::ShaderOperand _predicate;
        rg::ShaderOperand _iterator;
        std::vector<rg::ShaderInstruction> body;
    };
}

#endif //XENGINE_LOOPBUILDER_HPP