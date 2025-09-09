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

#include "compiledtree.hpp"

#include "xng/render/graph2/shader/nodes.hpp"

using namespace xng;

static const char *inputAttributePrefix = "in_";
static const char *outputAttributePrefix = "out_";
static const char *parameterPrefix = "param_";
static const char *bufferPrefix = "buffer_";
static const char *bufferArrayName = "data";
static const char *texturePrefix = "texture_";

CompiledNode createCompiledNode(std::shared_ptr<FGShaderNode> node, const FGShaderSource &source);

CompiledNode createCompiledNode(const FGNodeVector &node,
                                const std::shared_ptr<FGShaderNode> &nodePtr,
                                const FGShaderSource &source);

CompiledNode createCompiledNode(const FGNodeLiteral &node, const std::shared_ptr<FGShaderNode> &nodePtr);

CompiledNode createCompiledNode(const FGNodeAdd &node, const std::shared_ptr<FGShaderNode> &nodePtr);

CompiledNode createCompiledNode(const FGNodeMultiply &node, const std::shared_ptr<FGShaderNode> &nodePtr);

CompiledNode createCompiledNode(const FGNodeAttributeRead &node, const std::shared_ptr<FGShaderNode> &nodePtr);

CompiledNode createCompiledNode(const FGNodeAttributeWrite &node, const std::shared_ptr<FGShaderNode> &nodePtr);

CompiledNode createCompiledNode(const FGNodeBufferRead &node, const std::shared_ptr<FGShaderNode> &nodePtr, const FGShaderSource &source);

CompiledNode createCompiledNode(const FGNodeSubscript &node,
                                const std::shared_ptr<FGShaderNode> &nodePtr,
                                const FGShaderSource &source);

#endif //XENGINE_NODECOMPILER_HPP