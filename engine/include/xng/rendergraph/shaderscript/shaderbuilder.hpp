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

#ifndef XENGINE_SHADERBUILDER_HPP
#define XENGINE_SHADERBUILDER_HPP

#include <memory>

#include "xng/rendergraph/shader/shadernode.hpp"
#include "xng/rendergraph/shader/shaderstage.hpp"

namespace xng::ShaderScript {
    class ShaderNodeWrapper;

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

        [[nodiscard]] const std::vector<ShaderTextureArray> &getTextureArrays() const {
            return textureArrays;
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
        void If(const ShaderNodeWrapper &condition);

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
        void For(const ShaderNodeWrapper &loopVariable,
                 const ShaderNodeWrapper &loopStart,
                 const ShaderNodeWrapper &loopEnd,
                 const ShaderNodeWrapper &incrementor);

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
                      ShaderDataType returnType);

        void EndFunction();

        std::string getVariableName();

        void addNode(const std::unique_ptr<ShaderNode> &node);

        /**
         * Clears internal state and sets up the builder using the specified shader source data.
         *
         * @param stage
         * @param inputLayout
         * @param outputLayout
         * @param parameters
         * @param buffers
         * @param textureArrays
         * @param functions
         */
        void setup(ShaderStage::Type stage,
                   const ShaderAttributeLayout &inputLayout,
                   const ShaderAttributeLayout &outputLayout,
                   const std::unordered_map<std::string, ShaderDataType> &parameters,
                   const std::unordered_map<std::string, ShaderBuffer> &buffers,
                   const std::vector<ShaderTextureArray> &textureArrays,
                   const std::vector<ShaderFunction> &functions);

        /**
         * The previously recorded nodes are used as the body of the main function.
         * Clears the internally stored nodes.

         * @return
         */
        ShaderStage build();

    private:
        struct TreeNode {
            enum Type {
                ROOT,
                NODE,
                IF,
                FOR
            } type;

            bool processingElse = false;
            std::unique_ptr<ShaderNode> node;
            std::unique_ptr<ShaderNode> condition;
            std::unique_ptr<ShaderNode> loopVariable;
            std::unique_ptr<ShaderNode> initializer;
            std::unique_ptr<ShaderNode> loopEnd;
            std::unique_ptr<ShaderNode> incrementor;
            std::vector<std::shared_ptr<TreeNode> > defaultBranch;
            std::vector<std::shared_ptr<TreeNode> > falseBranch;
            TreeNode *parent{};
        };

        std::vector<std::unique_ptr<ShaderNode> > createNodes(TreeNode &node);

        ShaderStage::Type stage{};
        ShaderAttributeLayout inputLayout;
        ShaderAttributeLayout outputLayout;
        std::unordered_map<std::string, ShaderDataType> parameters;
        std::unordered_map<std::string, ShaderBuffer> buffers;
        std::vector<ShaderTextureArray> textureArrays;

        TreeNode *currentNode{};
        std::shared_ptr<TreeNode> rootNode;

        std::shared_ptr<TreeNode> functionRoot;
        std::vector<ShaderFunction> functions;

        size_t variableCounter = 0;

        ShaderFunction currentFunction;
    };
}

#endif //XENGINE_SHADERBUILDER_HPP
