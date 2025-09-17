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
#include "xng/rendergraph/shader/nodes/nodevariablecreate.hpp"
#include "xng/rendergraph/shader/nodes/nodevariable.hpp"
#include "xng/rendergraph/shader/nodes/nodeassign.hpp"

using namespace xng;

static const char *inputAttributePrefix = "in_";
static const char *outputAttributePrefix = "out_";
static const char *parameterPrefix = "param_";
static const char *bufferPrefix = "buffer_";
static const char *bufferArrayName = "data";
static const char *texturePrefix = "texture_";
static const char *drawID = "DRAW_ID";

/**
 * Compile the given node.
 *
 * @param node
 * @param source
 * @param functionName
 * @param prefix The prefix for lvalue nodes
 * @return
 */
std::string compileNode(const ShaderNode &node,
                        const Shader &source,
                        const std::string &functionName,
                        const std::string &prefix = "");

std::string compileLeafNode(const NodeVariableCreate &node,
                            const Shader &source,
                            const std::string &functionName,
                            const std::string &prefix);

std::string compileLeafNode(const NodeAssign &node,
                            const Shader &source,
                            const std::string &functionName,
                            const std::string &prefix);

std::string compileLeafNode(const NodeVariable &node);

std::string compileLeafNode(const NodeLiteral &node);

std::string compileLeafNode(const NodeArgument &node);

std::string compileLeafNode(const NodeAttributeInput &node);

std::string compileLeafNode(const NodeAttributeOutput &node);

std::string compileLeafNode(const NodeParameter &node);

std::string compileLeafNode(const NodeVector &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeMatrix &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeArray &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeTexture &node,
                            const Shader &source,
                            const std::string &functionName);

std::string compileLeafNode(const NodeTextureSample &node,
                            const Shader &source,
                            const std::string &functionName);

std::string compileLeafNode(const NodeTextureFetch &node,
                            const Shader &source,
                            const std::string &functionName);

std::string compileLeafNode(const NodeTextureSize &node,
                            const Shader &source,
                            const std::string &functionName);

std::string compileLeafNode(const NodeTextureFetch &node,
                            const Shader &source,
                            const std::string &functionName);

std::string compileLeafNode(const NodeBufferRead &node, const Shader &source,
                            const std::string &functionName);

std::string compileLeafNode(const NodeBufferWrite &node, const Shader &source,
                            const std::string &functionName,
                            const std::string &prefix);

std::string compileLeafNode(const NodeBufferSize &node, const Shader &source);

std::string compileLeafNode(const NodeAdd &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeSubtract &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeMultiply &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeDivide &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeEqual &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeNotEqual &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeGreater &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeLess &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeGreaterEqual &node, const Shader &source,
                            const std::string &functionName);

std::string compileLeafNode(const NodeLessEqual &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeAnd &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeOr &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeCall &node,
                            const Shader &source,
                            const std::string &functionName);

std::string compileLeafNode(const NodeReturn &node,
                            const Shader &source,
                            const std::string &functionName,
                            const std::string &prefix);

std::string compileLeafNode(const NodeBuiltin &node, const Shader &source, const std::string &functionName);

std::string compileLeafNode(const NodeSubscriptArray &node, const Shader &source,
                            const std::string &functionName);

std::string compileLeafNode(const NodeVectorSwizzle &node, const Shader &source,
                            const std::string &functionName);

std::string compileLeafNode(const NodeSubscriptMatrix &node, const Shader &source,
                            const std::string &functionName);

std::string compileLeafNode(const NodeBranch &node, const Shader &source, const std::string &functionName,
                            const std::string &prefix);

std::string compileLeafNode(const NodeLoop &node, const Shader &source, const std::string &functionName,
                            const std::string &prefix);

std::string compileLeafNode(const NodeVertexPosition &node, const Shader &source, const std::string &functionName,
                            const std::string &prefix);

#endif //XENGINE_NODECOMPILER_HPP
