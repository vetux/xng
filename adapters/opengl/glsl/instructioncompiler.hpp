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

#ifndef XENGINE_INSTRUCTIONCOMPILER_HPP
#define XENGINE_INSTRUCTIONCOMPILER_HPP

#include "xng/rendergraph/shader/shader.hpp"

using namespace xng;

static const char *inputAttributePrefix = "in_";
static const char *outputAttributePrefix = "out_";
static const char *parameterPrefix = "param_";
static const char *bufferPrefix = "buffer_";
static const char *bufferArrayName = "data";
static const char *texturePrefix = "texture_";
static const char *drawID = "DRAW_ID";

namespace InstructionCompiler {
    /**
     * Compile the given instruction.
     *
     * @param instruction
     * @param source
     * @param functionName
     * @param indent The tab indentation for lvalue instructions
     * @return
     */
    XENGINE_EXPORT std::string compile(const ShaderInstruction &instruction,
                                       const Shader &source,
                                       const std::string &functionName,
                                       const std::string &indent);

    XENGINE_EXPORT std::string compileOperand(const ShaderOperand &operand,
                                              const Shader &source,
                                              const std::string &functionName);

    XENGINE_EXPORT std::string compileDeclareVariable(const ShaderInstruction &instruction,
                                                      const Shader &source,
                                                      const std::string &functionName,
                                                      const std::string &indent);

    XENGINE_EXPORT std::string compileAssign(const ShaderInstruction &instruction,
                                             const Shader &source,
                                             const std::string &functionName,
                                             const std::string &indent);

    XENGINE_EXPORT std::string compileCreateVector(const ShaderInstruction &instruction,
                                                   const Shader &source,
                                                   const std::string &functionName,
                                                   const std::string &indent);

    XENGINE_EXPORT std::string compileCreateMatrix(const ShaderInstruction &instruction,
                                                   const Shader &source,
                                                   const std::string &functionName,
                                                   const std::string &indent);

    XENGINE_EXPORT std::string compileCreateArray(const ShaderInstruction &instruction,
                                                  const Shader &source,
                                                  const std::string &functionName,
                                                  const std::string &indent);

    XENGINE_EXPORT std::string compileCreateStruct(const ShaderInstruction &instruction,
                                                   const Shader &source,
                                                   const std::string &functionName,
                                                   const std::string &indent);

    XENGINE_EXPORT std::string compileTextureSample(const ShaderInstruction &instruction,
                                                    const Shader &source,
                                                    const std::string &functionName,
                                                    const std::string &indent);

    XENGINE_EXPORT std::string compileTextureFetch(const ShaderInstruction &instruction,
                                                   const Shader &source,
                                                   const std::string &functionName,
                                                   const std::string &indent);

    XENGINE_EXPORT std::string compileTextureSize(const ShaderInstruction &instruction,
                                                  const Shader &source,
                                                  const std::string &functionName,
                                                  const std::string &indent);

    XENGINE_EXPORT std::string compileBufferSize(const ShaderInstruction &instruction,
                                                 const Shader &source,
                                                 const std::string &functionName,
                                                 const std::string &indent);

    XENGINE_EXPORT std::string compileArithmetic(const ShaderInstruction &instruction,
                                                 const Shader &source,
                                                 const std::string &functionName,
                                                 const std::string &indent);

    XENGINE_EXPORT std::string compileCall(const ShaderInstruction &instruction,
                                           const Shader &source,
                                           const std::string &functionName,
                                           const std::string &indent);

    XENGINE_EXPORT std::string compileReturn(const ShaderInstruction &instruction,
                                             const Shader &source,
                                             const std::string &functionName,
                                             const std::string &indent);

    XENGINE_EXPORT std::string compileCallBuiltIn(const ShaderInstruction &instruction,
                                                  const Shader &source,
                                                  const std::string &functionName,
                                                  const std::string &indent);

    XENGINE_EXPORT std::string compileArraySubscript(const ShaderInstruction &instruction,
                                                     const Shader &source,
                                                     const std::string &functionName,
                                                     const std::string &indent);

    XENGINE_EXPORT std::string compileVectorSwizzle(const ShaderInstruction &instruction,
                                                    const Shader &source,
                                                    const std::string &functionName,
                                                    const std::string &indent);

    XENGINE_EXPORT std::string compileMatrixSubscript(const ShaderInstruction &instruction,
                                                      const Shader &source,
                                                      const std::string &functionName,
                                                      const std::string &indent);

    XENGINE_EXPORT std::string compileBranch(const ShaderInstruction &instruction,
                                             const Shader &source,
                                             const std::string &functionName,
                                             const std::string &indent);

    XENGINE_EXPORT std::string compileLoop(const ShaderInstruction &instruction,
                                           const Shader &source,
                                           const std::string &functionName,
                                           const std::string &indent);

    XENGINE_EXPORT std::string compileSetVertexPosition(const ShaderInstruction &instruction,
                                                        const Shader &source,
                                                        const std::string &functionName,
                                                        const std::string &indent);

    XENGINE_EXPORT std::string compileSetFragmentDepth(const ShaderInstruction &instruction,
                                                       const Shader &source,
                                                       const std::string &functionName,
                                                       const std::string &indent);

    XENGINE_EXPORT std::string compileSetLayer(const ShaderInstruction &instruction,
                                               const Shader &source,
                                               const std::string &functionName,
                                               const std::string &indent);

    XENGINE_EXPORT std::string compileEmitVertex(const ShaderInstruction &instruction,
                                                 const Shader &source,
                                                 const std::string &functionName,
                                                 const std::string &indent);

    XENGINE_EXPORT std::string compileEndPrimitive(const ShaderInstruction &instruction,
                                                   const Shader &source,
                                                   const std::string &functionName,
                                                   const std::string &indent);

    XENGINE_EXPORT std::string compileObjectElement(const ShaderInstruction &instruction,
                                                    const Shader &source,
                                                    const std::string &functionName,
                                                    const std::string &indent);
}

#endif //XENGINE_INSTRUCTIONCOMPILER_HPP
