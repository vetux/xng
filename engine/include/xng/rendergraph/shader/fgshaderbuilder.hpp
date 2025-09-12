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

#ifndef XENGINE_FGSHADERBUILDER_HPP
#define XENGINE_FGSHADERBUILDER_HPP

#include <memory>

#include "xng/rendergraph/shader/fgshadernode.hpp"
#include "xng/rendergraph/shader/fgshadersource.hpp"

namespace xng {
    class FGShaderNodeWrapper;

    class XENGINE_EXPORT FGShaderBuilder {
    public:
        /**
         * @return The static thread local builder instance.
         */
        static FGShaderBuilder &instance();

        [[nodiscard]] const FGAttributeLayout &getInputLayout() const {
            return inputLayout;
        }

        [[nodiscard]] const FGAttributeLayout &getOutputLayout() const {
            return outputLayout;
        }

        [[nodiscard]] const std::unordered_map<std::string, FGShaderValue> &getParameters() const {
            return parameters;
        }

        [[nodiscard]] const std::unordered_map<std::string, FGShaderBuffer> &getBuffers() const {
            return buffers;
        }

        [[nodiscard]] const std::unordered_map<std::string, FGTexture> &getTextures() const {
            return textures;
        }

        [[nodiscard]] const std::unordered_map<std::string, FGShaderFunction> &getFunctions() const {
            return functions;
        }

        const FGShaderFunction &getCurrentFunction() const {
            return currentFunction;
        }

        FGShaderBuilder() = default;

        /**
         * Subsequently added nodes are stored in the true branch of this conditional
         *
         * @param condition
         */
        void If(const FGShaderNodeWrapper &condition);

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
         * @param initializer The value the variable should be initialized to
         * @param condition The condition to check every iteration
         * @param incrementor The value added to the variable on every iteration
         */
        void For(const FGShaderNodeWrapper &loopVariable,
                 const FGShaderNodeWrapper &initializer,
                 const FGShaderNodeWrapper &condition,
                 const FGShaderNodeWrapper &incrementor);

        void EndFor();

        /**
         * Subsequently added nodes are used as the body of a function.
         *
         * @param name
         * @param arguments
         * @param returnType
         */
        void Function(const std::string &name,
                      const std::unordered_map<std::string, FGShaderValue> &arguments,
                      FGShaderValue returnType);

        void EndFunction();

        std::string getVariableName();

        void addNode(const std::unique_ptr<FGShaderNode> &node);

        /**
         * Clears internal state and sets up the builder using the specified shader source data.
         *
         * @param stage
         * @param inputLayout
         * @param outputLayout
         * @param parameters
         * @param buffers
         * @param textures
         * @param functions
         */
        void setup(FGShaderSource::ShaderStage stage,
                   const FGAttributeLayout &inputLayout,
                   const FGAttributeLayout &outputLayout,
                   const std::unordered_map<std::string, FGShaderValue> &parameters,
                   const std::unordered_map<std::string, FGShaderBuffer> &buffers,
                   const std::unordered_map<std::string, FGTexture> &textures,
                   const std::unordered_map<std::string, FGShaderFunction> &functions);

        /**
         * The previously recorded nodes are used as the body of the main function.
         * Clears the internally stored nodes.

         * @return
         */
        FGShaderSource build();

    private:
        struct TreeNode {
            enum Type {
                ROOT,
                NODE,
                IF,
                FOR
            } type;

            bool processingElse = false;
            std::unique_ptr<FGShaderNode> node;
            std::unique_ptr<FGShaderNode> condition;
            std::unique_ptr<FGShaderNode> loopVariable;
            std::unique_ptr<FGShaderNode> initializer;
            std::unique_ptr<FGShaderNode> incrementor;
            std::vector<std::shared_ptr<TreeNode> > defaultBranch;
            std::vector<std::shared_ptr<TreeNode> > falseBranch;
            std::shared_ptr<TreeNode> parent;
        };

        std::vector<std::unique_ptr<FGShaderNode> > createNodes(TreeNode &node);

        FGShaderSource::ShaderStage stage{};
        FGAttributeLayout inputLayout;
        FGAttributeLayout outputLayout;
        std::unordered_map<std::string, FGShaderValue> parameters;
        std::unordered_map<std::string, FGShaderBuffer> buffers;
        std::unordered_map<std::string, FGTexture> textures;

        std::shared_ptr<TreeNode> rootNode;
        std::shared_ptr<TreeNode> currentNode;

        std::shared_ptr<TreeNode> functionRoot;
        std::unordered_map<std::string, FGShaderFunction> functions;

        size_t variableCounter = 0;

        FGShaderFunction currentFunction;
    };
}

/*
        */
#endif //XENGINE_FGSHADERBUILDER_HPP
