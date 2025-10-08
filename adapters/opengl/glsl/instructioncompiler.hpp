/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/rendergraph/shader/shader.hpp"

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
XENGINE_EXPORT std::string compile(const ShaderInstruction &node,
                                   const Shader &source,
                                   const std::string &functionName,
                                   const std::string &prefix);

XENGINE_EXPORT std::string compileOperand(const ShaderOperand &operand,
                                          const Shader &source,
                                          const std::string &functionName);

XENGINE_EXPORT std::string compileDeclareVariable(const ShaderInstruction &node,
                                                  const Shader &source,
                                                  const std::string &functionName,
                                                  const std::string &prefix);

XENGINE_EXPORT std::string compileAssign(const ShaderInstruction &node,
                                         const Shader &source,
                                         const std::string &functionName,
                                         const std::string &prefix);

XENGINE_EXPORT std::string compileCreateVector(const ShaderInstruction &node,
                                               const Shader &source,
                                               const std::string &functionName,
                                               const std::string &prefix);

XENGINE_EXPORT std::string compileCreateMatrix(const ShaderInstruction &node,
                                               const Shader &source,
                                               const std::string &functionName,
                                               const std::string &prefix);

XENGINE_EXPORT std::string compileCreateArray(const ShaderInstruction &node,
                                              const Shader &source,
                                              const std::string &functionName,
                                              const std::string &prefix);

XENGINE_EXPORT std::string compileCreateStruct(const ShaderInstruction &node,
                                               const Shader &source,
                                               const std::string &functionName,
                                               const std::string &prefix);

XENGINE_EXPORT std::string compileTextureSample(const ShaderInstruction &node,
                                                const Shader &source,
                                                const std::string &functionName,
                                                const std::string &prefix);

XENGINE_EXPORT std::string compileTextureFetch(const ShaderInstruction &node,
                                               const Shader &source,
                                               const std::string &functionName,
                                               const std::string &prefix);

XENGINE_EXPORT std::string compileTextureSize(const ShaderInstruction &node,
                                              const Shader &source,
                                              const std::string &functionName,
                                              const std::string &prefix);

XENGINE_EXPORT std::string compileBufferSize(const ShaderInstruction &node,
                                             const Shader &source,
                                             const std::string &functionName,
                                             const std::string &prefix);

XENGINE_EXPORT std::string compileArithmetic(const ShaderInstruction &node,
                                             const Shader &source,
                                             const std::string &functionName,
                                             const std::string &prefix);

XENGINE_EXPORT std::string compileCall(const ShaderInstruction &node,
                                       const Shader &source,
                                       const std::string &functionName,
                                       const std::string &prefix);

XENGINE_EXPORT std::string compileReturn(const ShaderInstruction &node,
                                         const Shader &source,
                                         const std::string &functionName,
                                         const std::string &prefix);

XENGINE_EXPORT std::string compileCallBuiltIn(const ShaderInstruction &node,
                                              const Shader &source,
                                              const std::string &functionName,
                                              const std::string &prefix);

XENGINE_EXPORT std::string compileArraySubscript(const ShaderInstruction &node,
                                                 const Shader &source,
                                                 const std::string &functionName,
                                                 const std::string &prefix);

XENGINE_EXPORT std::string compileVectorSwizzle(const ShaderInstruction &node,
                                                const Shader &source,
                                                const std::string &functionName,
                                                const std::string &prefix);

XENGINE_EXPORT std::string compileMatrixSubscript(const ShaderInstruction &node,
                                                  const Shader &source,
                                                  const std::string &functionName,
                                                  const std::string &prefix);

XENGINE_EXPORT std::string compileBranch(const ShaderInstruction &node,
                                         const Shader &source,
                                         const std::string &functionName,
                                         const std::string &prefix);

XENGINE_EXPORT std::string compileLoop(const ShaderInstruction &node,
                                       const Shader &source,
                                       const std::string &functionName,
                                       const std::string &prefix);

XENGINE_EXPORT std::string compileSetVertexPosition(const ShaderInstruction &node,
                                                    const Shader &source,
                                                    const std::string &functionName,
                                                    const std::string &prefix);

XENGINE_EXPORT std::string compileSetFragmentDepth(const ShaderInstruction &node,
                                                   const Shader &source,
                                                   const std::string &functionName,
                                                   const std::string &prefix);

XENGINE_EXPORT std::string compileSetLayer(const ShaderInstruction &node,
                                           const Shader &source,
                                           const std::string &functionName,
                                           const std::string &prefix);

XENGINE_EXPORT std::string compileEmitVertex(const ShaderInstruction &node,
                                             const Shader &source,
                                             const std::string &functionName,
                                             const std::string &prefix);

XENGINE_EXPORT std::string compileEndPrimitive(const ShaderInstruction &node,
                                               const Shader &source,
                                               const std::string &functionName,
                                               const std::string &prefix);

XENGINE_EXPORT std::string compileObjectElement(const ShaderInstruction &node,
                                                const Shader &source,
                                                const std::string &functionName,
                                                const std::string &prefix);

#endif //XENGINE_NODECOMPILER_HPP
