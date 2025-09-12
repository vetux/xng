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

#ifndef XENGINE_NODECOMPILER_HPP
#define XENGINE_NODECOMPILER_HPP

#include "xng/rendergraph/shader/nodes.hpp"
#include "xng/rendergraph/shader/nodes/fgnodevariablecreate.hpp"
#include "xng/rendergraph/shader/nodes/fgnodevariable.hpp"
#include "xng/rendergraph/shader/nodes/fgnodeassign.hpp"

using namespace xng;

static const char *inputAttributePrefix = "in_";
static const char *outputAttributePrefix = "out_";
static const char *parameterPrefix = "param_";
static const char *bufferPrefix = "buffer_";
static const char *bufferArrayName = "data";
static const char *texturePrefix = "texture_";

/**
 * Compile the given node.
 *
 * @param node
 * @param source
 * @param functionName
 * @param prefix The prefix for lvalue nodes
 * @return
 */
std::string compileNode(const FGShaderNode &node,
                        const FGShaderSource &source,
                        const std::string &functionName,
                        const std::string &prefix = "");

std::string compileLeafNode(const FGNodeVariableCreate &node,
                            const FGShaderSource &source,
                            const std::string &functionName,
                            const std::string &prefix);

std::string compileLeafNode(const FGNodeAssign &node,
                            const FGShaderSource &source,
                            const std::string &functionName,
                            const std::string &prefix);

std::string compileLeafNode(const FGNodeVariable &node);

std::string compileLeafNode(const FGNodeLiteral &node);

std::string compileLeafNode(const FGNodeArgument &node);

std::string compileLeafNode(const FGNodeAttributeInput &node);

std::string compileLeafNode(const FGNodeAttributeOutput &node);

std::string compileLeafNode(const FGNodeParameter &node);

std::string compileLeafNode(const FGNodeVector &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeArray &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeTextureSample &node, const FGShaderSource &source,
                            const std::string &functionName);

std::string compileLeafNode(const FGNodeTextureSize &node);

std::string compileLeafNode(const FGNodeBufferRead &node, const FGShaderSource &source,
                            const std::string &functionName);

std::string compileLeafNode(const FGNodeBufferWrite &node, const FGShaderSource &source,
                            const std::string &functionName,
                            const std::string &prefix);

std::string compileLeafNode(const FGNodeBufferSize &node, const FGShaderSource &source);

std::string compileLeafNode(const FGNodeAdd &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeSubtract &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeMultiply &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeDivide &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeEqual &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeNotEqual &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeGreater &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeLess &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeGreaterEqual &node, const FGShaderSource &source,
                            const std::string &functionName);

std::string compileLeafNode(const FGNodeLessEqual &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeAnd &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeOr &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeCall &node,
                            const FGShaderSource &source,
                            const std::string &functionName);

std::string compileLeafNode(const FGNodeReturn &node,
                            const FGShaderSource &source,
                            const std::string &functionName,
                            const std::string &prefix);

std::string compileLeafNode(const FGNodeBuiltin &node, const FGShaderSource &source, const std::string &functionName);

std::string compileLeafNode(const FGNodeSubscriptArray &node, const FGShaderSource &source,
                            const std::string &functionName);

std::string compileLeafNode(const FGNodeSubscriptVector &node, const FGShaderSource &source,
                            const std::string &functionName);

std::string compileLeafNode(const FGNodeSubscriptMatrix &node, const FGShaderSource &source,
                            const std::string &functionName);

std::string compileLeafNode(const FGNodeBranch &node, const FGShaderSource &source, const std::string &functionName,
                            const std::string &prefix);

std::string compileLeafNode(const FGNodeLoop &node, const FGShaderSource &source, const std::string &functionName,
                            const std::string &prefix);

#endif //XENGINE_NODECOMPILER_HPP
