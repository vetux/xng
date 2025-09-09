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

#ifndef XENGINE_COMPILEDTREE_HPP
#define XENGINE_COMPILEDTREE_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "xng/render/graph2/shader/fgshadernode.hpp"

using namespace xng;

struct CompiledTree;

struct CompiledNode {
    std::shared_ptr<FGShaderNode> node;
    std::vector<std::variant<std::string, std::shared_ptr<FGShaderNode> > > content;

    CompiledNode() = default;

    explicit CompiledNode(const std::shared_ptr<FGShaderNode> &node)
        : node(node) {
    }

    std::string getValueCode(const CompiledTree &tree) const;
};

struct CompiledTree {
    std::unordered_map<std::shared_ptr<FGShaderNode>, std::string> variables;
    std::unordered_map<std::shared_ptr<FGShaderNode>, CompiledNode> nodes;
};

inline std::string CompiledNode::getValueCode(const CompiledTree &tree) const {
    std::string ret;
    for (auto var: content) {
        if (var.index() == 0) {
            ret += std::get<std::string>(var);
        } else {
            auto sourceNode = std::get<std::shared_ptr<FGShaderNode> >(var);
            if (tree.variables.find(sourceNode) == tree.variables.end()) {
                // Inline
                ret += tree.nodes.at(sourceNode).getValueCode(tree);
            } else {
                // Use Variable
                ret += tree.variables.at(sourceNode);
            }
        }
    }
    return ret;
}

#endif //XENGINE_COMPILEDTREE_HPP