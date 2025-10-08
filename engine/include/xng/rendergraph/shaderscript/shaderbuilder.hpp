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

#ifndef XENGINE_SHADERBUILDER_HPP
#define XENGINE_SHADERBUILDER_HPP

#include <memory>

#include "xng/rendergraph/shader/shaderinstruction.hpp"
#include "xng/rendergraph/shader/shader.hpp"
#include "xng/rendergraph/shader/shaderinstructionfactory.hpp"

namespace xng::ShaderScript {
    class ShaderObject;

    class XENGINE_EXPORT ShaderBuilder {
    public:
        /**
         * @return The static thread local builder instance.
         */
        static ShaderBuilder &instance();

        [[nodiscard]] const ShaderAttributeLayout &getInputLayout() const {
            return inputLayout;
        }

        [[nodiscard]] const ShaderAttributeLayout &getOutputLayout() const {
            return outputLayout;
        }

        [[nodiscard]] const std::unordered_map<std::string, ShaderDataType> &getParameters() const {
            return parameters;
        }

        [[nodiscard]] const std::unordered_map<std::string, ShaderBuffer> &getBuffers() const {
            return buffers;
        }

        [[nodiscard]] const std::unordered_map<std::string, ShaderTextureArray> &getTextureArrays() const {
            return textureArrays;
        }

        const std::vector<ShaderStruct> &getTypeDefinitions() const {
            return typeDefinitions;
        }

        [[nodiscard]] const std::vector<ShaderFunction> &getFunctions() const {
            return functions;
        }

        const ShaderFunction &getCurrentFunction() const {
            return currentFunction;
        }

        ShaderBuilder() = default;

        /**
         * Subsequently added nodes are stored in the true branch of this conditional
         *
         * @param condition
         */
        void If(const ShaderObject &condition);

        /**
         * Subsequently added nodes are stored in the false branch of this conditional
         */
        void Else();

        /**
         * Finish the conditional branch.
         */
        void EndIf();

        /**
         * Begin a for loop
         *
         * @param loopVariable The variable to perform looping on
         * @param loopStart The value the variable should be initialized to
         * @param loopEnd The value the variable should be looped up to (loopVariable <= loopEnd)
         * @param incrementor The value added to the variable on every iteration
         */
        void For(const ShaderObject &loopVariable,
                 const ShaderObject &loopStart,
                 const ShaderObject &loopEnd,
                 const ShaderObject &incrementor);

        void EndFor();

        /**
         * Subsequently added nodes are used as the body of a function.
         *
         * @param name
         * @param arguments
         * @param returnType
         */
        void Function(const std::string &name,
                      const std::vector<ShaderFunction::Argument> &arguments,
                      ShaderFunction::ReturnType returnType);

        void EndFunction();

        std::string getVariableName();

        void addInstruction(const ShaderInstruction &inst);

        /**
         * Clears internal state.
         *
         * @param stage
         */
        void setup(Shader::Stage stage);

        void addInput(const std::string &name, const ShaderDataType &type) {
            inputLayout.addElement(name, type);
        }

        void addOutput(const std::string &name, const ShaderDataType &type) {
            outputLayout.addElement(name, type);
        }

        void addParameter(const std::string &name, const ShaderDataType &type) {
            parameters.emplace(name, type);
        }

        void addBuffer(const std::string &name, const ShaderBuffer &buffer) {
            buffers.emplace(name, buffer);
        }

        void addTextureArray(const std::string &name, const ShaderTextureArray &textureArray) {
            textureArrays.emplace(name, textureArray);
        }

        void addTypeDefinition(const ShaderStruct &type) {
            for (auto &t: typeDefinitions) {
                if (t.name == type.name) {
                    throw std::runtime_error("Type definition already exists");
                }
            }
            typeDefinitions.emplace_back(type);
        }

        void setGeometryInput(Primitive input) {
            geometryInput = input;
        }

        void setGeometryOutput(Primitive output, size_t maxVertices) {
            geometryOutput = output;
            geometryMaxVertices = maxVertices;
        }

        /**
         * The previously recorded nodes are used as the body of the main function.
         * Clears the internally stored nodes.

         * @return
         */
        Shader build();

    private:
        struct TreeNode {
            enum Type {
                ROOT,
                NODE,
                IF,
                FOR
            } type;

            bool processingElse = false;
            ShaderInstruction node;
            ShaderOperand condition;
            ShaderOperand loopVariable;
            ShaderOperand initializer;
            ShaderOperand loopEnd;
            ShaderOperand incrementor;
            std::vector<std::shared_ptr<TreeNode> > defaultBranch;
            std::vector<std::shared_ptr<TreeNode> > falseBranch;
            TreeNode *parent{};
        };

        std::vector<ShaderInstruction> createNodes(TreeNode &node);

        Shader::Stage stage{};
        ShaderAttributeLayout inputLayout;
        ShaderAttributeLayout outputLayout;

        Primitive geometryInput;
        Primitive geometryOutput;
        size_t geometryMaxVertices{};

        std::unordered_map<std::string, ShaderDataType> parameters;
        std::unordered_map<std::string, ShaderBuffer> buffers;
        std::unordered_map<std::string, ShaderTextureArray> textureArrays;
        std::vector<ShaderStruct> typeDefinitions;

        TreeNode *currentNode{};
        std::shared_ptr<TreeNode> rootNode;

        std::shared_ptr<TreeNode> functionRoot;
        std::vector<ShaderFunction> functions;

        size_t variableCounter = 0;

        ShaderFunction currentFunction;
    };
}

#endif //XENGINE_SHADERBUILDER_HPP
