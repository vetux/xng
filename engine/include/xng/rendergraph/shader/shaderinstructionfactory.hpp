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

#ifndef XENGINE_SHADERINSTRUCTIONFACTORY_HPP
#define XENGINE_SHADERINSTRUCTIONFACTORY_HPP

#include "xng/rendergraph/shader/shaderinstruction.hpp"

/**
 * The ShaderInstructionFactory defines the format of instructions.
 */
namespace xng::ShaderInstructionFactory {
    XENGINE_EXPORT ShaderInstruction declareVariable(std::string name,
                                                     std::variant<ShaderDataType, ShaderStructTypeName> type,
                                                     ShaderOperand value = {});

    XENGINE_EXPORT ShaderInstruction assign(ShaderOperand target, ShaderOperand value);


    XENGINE_EXPORT ShaderInstruction branch(ShaderOperand condition,
                                            std::vector<ShaderInstruction> trueBranch,
                                            std::vector<ShaderInstruction> falseBranch);

    XENGINE_EXPORT ShaderInstruction loop(ShaderOperand initializer,
                                          ShaderOperand predicate,
                                          ShaderOperand iterator,
                                          std::vector<ShaderInstruction> body);

    XENGINE_EXPORT ShaderInstruction call(std::string name, std::vector<ShaderOperand> arguments);

    XENGINE_EXPORT ShaderInstruction ret(ShaderOperand returnValue = {});

    XENGINE_EXPORT ShaderInstruction emitVertex();

    XENGINE_EXPORT ShaderInstruction endPrimitive();

    XENGINE_EXPORT ShaderInstruction setFragmentDepth(ShaderOperand depth);

    XENGINE_EXPORT ShaderInstruction setLayer(ShaderOperand layer);

    XENGINE_EXPORT ShaderInstruction setVertexPosition(ShaderOperand position);

    XENGINE_EXPORT ShaderInstruction vectorSwizzle(ShaderOperand vector,
                                                   std::vector<ShaderInstruction::VectorComponent> components);

    XENGINE_EXPORT ShaderInstruction arraySubscript(ShaderOperand array, ShaderOperand index);

    XENGINE_EXPORT ShaderInstruction matrixSubscript(ShaderOperand matrix,
                                                     ShaderOperand column,
                                                     ShaderOperand row = {});

    XENGINE_EXPORT ShaderInstruction objectMember(ShaderOperand object, std::string member);

    XENGINE_EXPORT ShaderInstruction createArray(ShaderDataType elementType,
                                                 std::vector<ShaderOperand> elements);

    XENGINE_EXPORT ShaderInstruction createMatrix(ShaderDataType elementType,
                                                  ShaderOperand x,
                                                  ShaderOperand y = {},
                                                  ShaderOperand z = {},
                                                  ShaderOperand w = {});

    XENGINE_EXPORT ShaderInstruction createVector(ShaderDataType elementType,
                                                  ShaderOperand x,
                                                  ShaderOperand y = {},
                                                  ShaderOperand z = {},
                                                  ShaderOperand w = {});

    XENGINE_EXPORT ShaderInstruction createStruct(ShaderStructTypeName typeName);

    /**
     * Fetch a texel of a TEXTURE_2D.
     *
     * UV Coordinates origin is upper left.
     *
     * @param texture
     * @param coordinate A 2d integer vector specifying the texel coordinates
     * @param lod
     * @return
     */
    XENGINE_EXPORT ShaderInstruction textureFetch(ShaderOperand texture,
                                                  ShaderOperand coordinate,
                                                  ShaderOperand lod = {});

    /**
     * Fetch a texel of a TEXTURE_2D_ARRAY.
     *
     * UV Coordinates origin is upper left.
     *
     * @param texture
     * @param coordinate A 3d integer vector specifying the texel coordinates in x/y and the array layer in z.
     * @param lod
     * @return
     */
    XENGINE_EXPORT ShaderInstruction textureFetchArray(ShaderOperand texture,
                                                       ShaderOperand coordinate,
                                                       ShaderOperand lod = {});

    /**
     * Fetch a texel of a TEXTURE_2D_MULTISAMPLE texture.
     *
     * UV Coordinates origin is upper left.
     *
     * @param texture
     * @param coordinate A 2d integer vector specifying the texel coordinates
     * @param sample The sample index
     * @return
     */
    XENGINE_EXPORT ShaderInstruction textureFetchMS(ShaderOperand texture,
                                                    ShaderOperand coordinate,
                                                    ShaderOperand sample = {});

    /**
     * Fetch a texel of a TEXTURE_2D_MULTISAMPLE_ARRAY texture.
     *
     * UV Coordinates origin is upper left.
     *
     * @param texture
     * @param coordinate A 3d integer vector specifying the texel coordinates in x/y and the array layer in z.
     * @param sample The sample index
     * @return
     */
    XENGINE_EXPORT ShaderInstruction textureFetchMSArray(ShaderOperand texture,
                                                         ShaderOperand coordinate,
                                                         ShaderOperand sample = {});

    /**
     * Sample a TEXTURE_2D.
     *
     * UV Coordinates origin is upper left.
     *
     * @param texture
     * @param coordinate A 2d float vector specifying the uv coordinates.
     * @param bias Optional bias to apply during level-of-detail computation.
     * @return
     */
    XENGINE_EXPORT ShaderInstruction textureSample(ShaderOperand texture,
                                                   ShaderOperand coordinate,
                                                   ShaderOperand bias = {});

    /**
     * Sample a TEXTURE_2D_ARRAY, TEXTURE_2D_MULTISAMPLE_ARRAY texture.
     *
     * UV Coordinates origin is upper left.
     *
     * @param texture
     * @param coordinate A 3d float vector specifying the uv coordinates in x/y and the array layer in z.
     * @param bias Optional bias to apply during level-of-detail computation.
     * @return
     */
    XENGINE_EXPORT ShaderInstruction textureSampleArray(ShaderOperand texture,
                                                        ShaderOperand coordinate,
                                                        ShaderOperand bias = {});

    /**
     * Sample a TEXTURE_CUBE_MAP texture.
     *
     * The sampled texel is computed by interpreting the coordinate in left-hand coordinate space
     * with +Y pointing up matching the OpenGL convention.
     *
     * @param texture
     * @param coordinate A 3d float vector specifying the point to sample.
     * @param bias Optional bias to apply during level-of-detail computation.
     * @return
     */
    XENGINE_EXPORT ShaderInstruction textureSampleCubeMap(ShaderOperand texture,
                                                          ShaderOperand coordinate,
                                                          ShaderOperand bias = {});

    /**
     * Sample a TEXTURE_CUBE_MAP_ARRAY texture.
     *
     * The sampled texel is computed by interpreting the coordinate in left-hand coordinate space
     * with +Y pointing up matching the OpenGL convention.
     *
     * @param texture
     * @param coordinate A 4d float vector specifying the point to sample in xyz and the array layer in w.
     * @param bias Optional bias to apply during level-of-detail computation.
     * @return
     */
    XENGINE_EXPORT ShaderInstruction textureSampleCubeMapArray(ShaderOperand texture,
                                                               ShaderOperand coordinate,
                                                               ShaderOperand bias = {});

    XENGINE_EXPORT ShaderInstruction textureSize(ShaderOperand texture, ShaderOperand lod = {});

    XENGINE_EXPORT ShaderInstruction bufferSize(std::string name);

    XENGINE_EXPORT ShaderInstruction add(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction subtract(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction multiply(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction divide(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction logicalAnd(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction logicalOr(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction compareGreaterEqual(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction compareGreater(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction compareLessEqual(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction compareLess(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction compareEqual(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction compareNotEqual(ShaderOperand left, ShaderOperand right);

    XENGINE_EXPORT ShaderInstruction abs(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction sin(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction cos(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction tan(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction asin(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction acos(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction atan(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction pow(ShaderOperand value, ShaderOperand exponent);

    XENGINE_EXPORT ShaderInstruction exp(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction log(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction sqrt(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction inverseSqrt(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction floor(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction ceil(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction round(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction fract(ShaderOperand value);

    XENGINE_EXPORT ShaderInstruction mod(ShaderOperand value, ShaderOperand modulus);

    XENGINE_EXPORT ShaderInstruction min(ShaderOperand x, ShaderOperand y);

    XENGINE_EXPORT ShaderInstruction max(ShaderOperand x, ShaderOperand y);

    XENGINE_EXPORT ShaderInstruction clamp(ShaderOperand value,
                                           ShaderOperand min,
                                           ShaderOperand max);

    XENGINE_EXPORT ShaderInstruction mix(ShaderOperand x, ShaderOperand y,
                                         ShaderOperand a);

    XENGINE_EXPORT ShaderInstruction step(ShaderOperand edge, ShaderOperand x);

    XENGINE_EXPORT ShaderInstruction smoothstep(ShaderOperand edge0,
                                                ShaderOperand edge1,
                                                ShaderOperand x);

    XENGINE_EXPORT ShaderInstruction dot(ShaderOperand x, ShaderOperand y);

    XENGINE_EXPORT ShaderInstruction cross(ShaderOperand x, ShaderOperand y);

    XENGINE_EXPORT ShaderInstruction normalize(ShaderOperand x);

    XENGINE_EXPORT ShaderInstruction length(ShaderOperand x);

    XENGINE_EXPORT ShaderInstruction distance(ShaderOperand x, ShaderOperand y);

    XENGINE_EXPORT ShaderInstruction reflect(ShaderOperand I, ShaderOperand N);

    XENGINE_EXPORT ShaderInstruction refract(ShaderOperand I,
                                             ShaderOperand N,
                                             ShaderOperand eta);

    XENGINE_EXPORT ShaderInstruction faceForward(ShaderOperand N,
                                                 ShaderOperand I,
                                                 ShaderOperand Nref);

    XENGINE_EXPORT ShaderInstruction transpose(ShaderOperand x);

    XENGINE_EXPORT ShaderInstruction inverse(ShaderOperand x);
}

#endif //XENGINE_SHADERINSTRUCTIONFACTORY_HPP
