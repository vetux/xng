/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
        void BeginFor(ShaderOperand loopVariable, ShaderOperand initializer, ShaderOperand predicate, ShaderOperand iterator)
        {
            if (loopVariable.type != ShaderOperand::Variable) throw std::runtime_error("Invalid loop variable");
            if (initializer.type == ShaderOperand::Variable) throw std::runtime_error("Invalid loop initializer");
            if (iterator.type == ShaderOperand::Variable) throw std::runtime_error("Invalid loop iterator");

            BlockScope::get().addInstruction(ShaderInstructionFactory::assign(loopVariable, initializer));
            _initializer = std::move(initializer);
            _iterator = ShaderOperand::instruction(ShaderInstructionFactory::assign(loopVariable, iterator));
            _predicate = std::move(predicate);
            blockScope = std::make_unique<BlockScope>();
        }

        void EndFor()
        {
            if (blockScope == nullptr) throw std::runtime_error("EndFor without preceding BeginFor");
            body = blockScope->buildInstructionStream();
            blockScope = nullptr;
        }

        ShaderInstruction build() const
        {
            if (blockScope != nullptr) throw std::runtime_error("Incomplete loop block");
            return ShaderInstructionFactory::loop(_initializer, _predicate, _iterator, body);
        }

    private:
        std::unique_ptr<BlockScope> blockScope;

        ShaderOperand _initializer;
        ShaderOperand _predicate;
        ShaderOperand _iterator;
        std::vector<ShaderInstruction> body;
    };
}

#endif //XENGINE_LOOPBUILDER_HPP