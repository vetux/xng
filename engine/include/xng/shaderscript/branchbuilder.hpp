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

#ifndef XENGINE_BRANCHBUILDER_HPP
#define XENGINE_BRANCHBUILDER_HPP

#include <vector>
#include <memory>

#include "xng/shaderscript/blockscope.hpp"

#include "xng/rendergraph/shader/shaderinstructionfactory.hpp"

namespace xng::ShaderScript
{
    class XENGINE_EXPORT BranchBuilder
    {
    public:
        void RecordIf(rg::ShaderOperand cond)
        {
            condition = std::move(cond);
            block = std::make_unique<BlockScope>();
            recordingTrueBranch = true;
        }

        void RecordElse()
        {
            if (block == nullptr) throw std::runtime_error("RecordElse without preceding RecordIf call.");
            recordingTrueBranch = false;
            trueBranch = block->buildInstructionStream();
            block.reset();
            block = std::make_unique<BlockScope>();
        }

        void Finish()
        {
            if (block == nullptr) throw std::runtime_error("Finish without preceding RecordIf call.");
            if (recordingTrueBranch)
                trueBranch = block->buildInstructionStream();
            else
                falseBranch = block->buildInstructionStream();
            block = nullptr;
        }

        rg::ShaderInstruction build()
        {
            if (block != nullptr || trueBranch.empty()) throw std::runtime_error("Uninitialized branch");
            return rg::ShaderInstructionFactory::branch(condition, trueBranch, falseBranch);
        }

    private:
        bool recordingTrueBranch = false;
        std::unique_ptr<BlockScope> block;
        rg::ShaderOperand condition;
        std::vector<rg::ShaderInstruction> trueBranch;
        std::vector<rg::ShaderInstruction> falseBranch;
    };
}

#endif //XENGINE_BRANCHBUILDER_HPP
