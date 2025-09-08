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

#include "xng/render/graph2/shader/nodes.hpp"

namespace xng {
    std::shared_ptr<FGShaderNode> FGShaderBuilder::literal(const FGShaderLiteral &value) {
        auto node = std::make_shared<FGNodeLiteral>(value);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::vector(const std::shared_ptr<FGShaderNode> &x,
                                                          const std::shared_ptr<FGShaderNode> &y,
                                                          const std::shared_ptr<FGShaderNode> &z,
                                                          const std::shared_ptr<FGShaderNode> &w) {
        auto node = std::make_shared<FGNodeVector>();
        connectNodes(node->x, node, x);
        connectNodes(node->y, node, y);
        connectNodes(node->z, node, z);
        connectNodes(node->w, node, w);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::attributeRead(int32_t attributeIndex) {
        auto node = std::make_shared<FGNodeAttributeRead>(attributeIndex);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::attributeWrite(int32_t attributeIndex,
                                                                  const std::shared_ptr<FGShaderNode> &value) {
        auto node = std::make_shared<FGNodeAttributeWrite>(attributeIndex);
        connectNodes(node->value, node, value);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::parameterRead(const std::string &parameter_name) {
        auto node = std::make_shared<FGNodeParameterRead>(parameter_name);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::textureSample(const std::string &textureName,
                                                                 const std::shared_ptr<FGShaderNode> &coordinate,
                                                                 const std::shared_ptr<FGShaderNode> &bias) {
        auto node = std::make_shared<FGNodeTextureSample>(textureName);
        connectNodes(node->coordinate, node, coordinate);
        connectNodes(node->bias, node, bias);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::textureSize(const std::string &textureName) {
        auto node = std::make_shared<FGNodeTextureSize>(textureName);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::bufferRead(const std::string &bufferName,
                                                              const std::string &elementName) {
        auto node = std::make_shared<FGNodeBufferRead>(bufferName, elementName);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::bufferWrite(const std::string &bufferName,
                                                               const std::string &elementName,
                                                               const std::shared_ptr<FGShaderNode> &value) {
        auto node = std::make_shared<FGNodeBufferWrite>(bufferName, elementName);
        connectNodes(node->value, node, value);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::add(const std::shared_ptr<FGShaderNode> &left,
                                                       const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeAdd>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::subtract(const std::shared_ptr<FGShaderNode> &left,
                                                            const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeSubtract>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::multiply(const std::shared_ptr<FGShaderNode> &left,
                                                            const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeMultiply>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::divide(const std::shared_ptr<FGShaderNode> &left,
                                                          const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeDivide>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::compareEqual(const std::shared_ptr<FGShaderNode> &left,
                                                                const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeEqual>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::compareNotEqual(const std::shared_ptr<FGShaderNode> &left,
                                                                   const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeNotEqual>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::compareGreater(const std::shared_ptr<FGShaderNode> &left,
                                                                  const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeGreater>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::compareLess(const std::shared_ptr<FGShaderNode> &left,
                                                               const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeLess>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::compareGreaterEqual(const std::shared_ptr<FGShaderNode> &left,
                                                                       const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeGreaterEqual>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::compareLessEqual(const std::shared_ptr<FGShaderNode> &left,
                                                                    const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeLessEqual>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::logicalAnd(const std::shared_ptr<FGShaderNode> &left,
                                                              const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeAnd>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::logicalOr(const std::shared_ptr<FGShaderNode> &left,
                                                             const std::shared_ptr<FGShaderNode> &right) {
        auto node = std::make_shared<FGNodeOr>();
        connectNodes(node->left, node, left);
        connectNodes(node->right, node, right);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::normalize(const std::shared_ptr<FGShaderNode> &value) {
        auto node = std::make_shared<FGNodeNormalize>();
        connectNodes(node->value, node, value);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::subscript(const std::shared_ptr<FGShaderNode> &value,
                                                             const std::shared_ptr<FGShaderNode> &row,
                                                             const std::shared_ptr<FGShaderNode> &column) {
        auto node = std::make_shared<FGNodeSubscript>();
        connectNodes(node->value, node, value);
        connectNodes(node->row, node, row);
        connectNodes(node->column, node, column);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::branch(const std::shared_ptr<FGShaderNode> &condition,
                                                          const std::shared_ptr<FGShaderNode> &trueBranch,
                                                          const std::shared_ptr<FGShaderNode> &falseBranch) {
        auto node = std::make_shared<FGNodeBranch>();
        connectNodes(node->condition, node, condition);
        connectNodes(node->trueBranch, node, trueBranch);
        connectNodes(node->falseBranch, node, falseBranch);
        nodes.emplace_back(node);
        return node;
    }

    std::shared_ptr<FGShaderNode> FGShaderBuilder::loop(const std::shared_ptr<FGShaderNode> &iterationStart,
                                                        const std::shared_ptr<FGShaderNode> &iterationEnd,
                                                        const std::shared_ptr<FGShaderNode> &iterationStep) {
        auto node = std::make_shared<FGNodeLoop>();
        connectNodes(node->iterationStart, node, iterationStart);
        connectNodes(node->iterationEnd, node, iterationEnd);
        connectNodes(node->iterationStep, node, iterationStep);
        nodes.emplace_back(node);
        return node;
    }

    FGShaderSource FGShaderBuilder::build(FGShaderSource::ShaderStage stage,
                                          const FGAttributeLayout &inputLayout,
                                          const FGAttributeLayout &outputLayout,
                                          const std::unordered_map<std::string, FGShaderValue> &parameters,
                                          const std::unordered_map<std::string, FGShaderBuffer> &buffers,
                                          const std::unordered_map<std::string, FGTexture> &textures) {
        return {stage, inputLayout, outputLayout, parameters, buffers, textures, nodes};
    }

    void FGShaderBuilder::connectNodes(FGShaderNodeInput &targetInput,
                                       const std::shared_ptr<FGShaderNode> &target,
                                       const std::shared_ptr<FGShaderNode> &source) {
        if (source != nullptr) {
            targetInput.source = source;
            source->getOutput().consumers.insert(target);
        }
    }
}
