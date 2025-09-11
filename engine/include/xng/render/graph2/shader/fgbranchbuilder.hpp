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

#ifndef XENGINE_FGBRANCHBUILDER_HPP
#define XENGINE_FGBRANCHBUILDER_HPP
#include "fgshaderbuilder.hpp"

namespace xng {
    class FGBranchBuilder {
    public:
        ~FGBranchBuilder() = default;

        void If(const std::unique_ptr<FGShaderNode> &condition) {
            stage = STAGE_If;
            branchCondition = condition->copy();
            trueBranch.clear();
            falseBranch.clear();
        }

        void Else() {
            stage = STAGE_Else;
        }

        void add(const std::unique_ptr<FGShaderNode> &node) {
            switch (stage) {
                case STAGE_If:
                    trueBranch.push_back(node->copy());
                    break;
                case STAGE_Else:
                    falseBranch.push_back(node->copy());
                    break;
                default:
                    throw std::runtime_error("Invalid stage");
            }
        }

        void EndIf() {
            stage = STAGE_None;
        }

        std::unique_ptr<FGShaderNode> build(FGShaderBuilder &builder) {
            if (stage != STAGE_None) {
                throw std::runtime_error("Invalid branch");
            }
            return builder.branch(branchCondition, trueBranch, falseBranch);
        }

    private:
        enum Stage {
            STAGE_None,
            STAGE_If,
            STAGE_Else,
        } stage;

        std::unique_ptr<FGShaderNode> branchCondition;
        std::vector<std::unique_ptr<FGShaderNode> > trueBranch;
        std::vector<std::unique_ptr<FGShaderNode> > falseBranch;
    };
}
#endif //XENGINE_FGBRANCHBUILDER_HPP
