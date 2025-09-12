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

#ifndef XENGINE_NODEBRANCH_HPP
#define XENGINE_NODEBRANCH_HPP

#include "xng/rendergraph/shader/shadernode.hpp"

namespace xng {
    struct NodeBranch final : ShaderNode {
        std::unique_ptr<ShaderNode> condition;
        std::vector<std::unique_ptr<ShaderNode> > trueBranch;
        std::vector<std::unique_ptr<ShaderNode> > falseBranch;

        NodeBranch(std::unique_ptr<ShaderNode> condition,
                     std::vector<std::unique_ptr<ShaderNode> > true_branch,
                     std::vector<std::unique_ptr<ShaderNode> > false_branch)
            : condition(std::move(condition)),
              trueBranch(std::move(true_branch)),
              falseBranch(std::move(false_branch)) {
        }

        NodeType getType() const override {
            return BRANCH;
        }

        std::unique_ptr<ShaderNode> copy() const override {
            std::vector<std::unique_ptr<ShaderNode> > trueBranchCopy;
            trueBranchCopy.reserve(trueBranch.size());
            for (auto &node: trueBranch) {
                trueBranchCopy.push_back(node->copy());
            }
            std::vector<std::unique_ptr<ShaderNode> > falseBranchCopy;
            falseBranchCopy.reserve(falseBranch.size());
            for (auto &node: falseBranch) {
                falseBranchCopy.push_back(node->copy());
            }
            return std::make_unique<NodeBranch>(condition->copy(),
                                                  std::move(trueBranchCopy),
                                                  std::move(falseBranchCopy));
        }
    };
}

#endif //XENGINE_NODEBRANCH_HPP
