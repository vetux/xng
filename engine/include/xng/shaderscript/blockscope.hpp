/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_SHADERBLOCKSCOPE_HPP
#define XENGINE_SHADERBLOCKSCOPE_HPP

#include <vector>

#include "xng/rendergraph/shader/shaderinstructionfactory.hpp"

namespace xng::ShaderScript
{
    using namespace xng::rendergraph;

    /**
     * Block scope represents the scope into which shader operations are recorded.
     * This may be a function body, loop body, or branch path.
     */
    class BlockScope
    {
    public:
        XENGINE_EXPORT static std::vector<BlockScope*>& getStack();
        XENGINE_EXPORT static BlockScope*& getCurrent();
        XENGINE_EXPORT static int& getObjectCounter();

        BlockScope()
        {
            if (getCurrent() != nullptr)
            {
                getStack().push_back(getCurrent());
            }
            getCurrent() = this;
        }

        virtual ~BlockScope()
        {
            getCurrent() = nullptr;
            if (!getStack().empty())
            {
                getCurrent() = getStack().back();
                getStack().pop_back();
            }
        }

        static BlockScope& get()
        {
            if (getCurrent() == nullptr)
            {
                throw std::runtime_error("Current block scope uninitialized");
            }
            return *getCurrent();
        }

        virtual void addInstruction(const ShaderInstruction& inst)
        {
            instructions.emplace_back(inst);
        }

        ShaderOperand registerObject(ShaderDataType type, ShaderOperand initializer)
        {
            const auto objectID = getObjectCounter()++;
            auto varName = "tmp" + std::to_string(objectID);
            objects.insert({varName, {std::move(type), std::move(initializer)}});
            return ShaderOperand::variable(varName);
        }

        std::vector<ShaderInstruction> buildInstructionStreamRecursive(
            const std::vector<ShaderInstruction>& inputInstructions,
            std::set<std::string>& initializedVariables)
        {
            std::vector<ShaderInstruction> ret;
            for (auto inst : inputInstructions)
            {
                // Patch operands (recursively handles nested Instruction operands)
                for (auto& op : inst.operands)
                {
                    patchOperand(op, ret, initializedVariables);
                }

                // Patch branch / loop blocks
                switch (inst.code)
                {
                case ShaderInstruction::Branch:
                    {
                        // Pre-declare any promoted variables referenced inside branches
                        // so they are visible across both branches and after the branch
                        auto& trueBranch = std::get<std::vector<ShaderInstruction>>(inst.data.at(0));
                        auto& falseBranch = std::get<std::vector<ShaderInstruction>>(inst.data.at(1));
                        std::set<std::string> referencedVars;
                        collectVariableReferences(trueBranch, referencedVars);
                        collectVariableReferences(falseBranch, referencedVars);
                        emitPendingDeclarations(referencedVars, ret, initializedVariables);

                        // Patch true branch
                        auto rInst = buildInstructionStreamRecursive(trueBranch, initializedVariables);
                        inst.data.at(0) = rInst;

                        // Patch false branch
                        rInst = buildInstructionStreamRecursive(falseBranch, initializedVariables);
                        inst.data.at(1) = rInst;

                        break;
                    }
                case ShaderInstruction::Loop:
                    {
                        // Pre-declare any promoted variables referenced inside the loop
                        auto& body = std::get<std::vector<ShaderInstruction>>(inst.data.at(0));
                        std::set<std::string> referencedVars;
                        collectVariableReferences(body, referencedVars);
                        emitPendingDeclarations(referencedVars, ret, initializedVariables);

                        // Patch body
                        auto rInst = buildInstructionStreamRecursive(body, initializedVariables);
                        inst.data.at(0) = rInst;
                        break;
                    }
                default:
                    break;
                }

                // Insert patched instruction
                ret.emplace_back(std::move(inst));
            }
            return ret;
        }

        std::vector<ShaderInstruction> buildInstructionStream()
        {
            promoteMultiReferencedObjects();
            std::set<std::string> initializedVariables;

            auto ret = buildInstructionStreamRecursive(instructions, initializedVariables);
            return ret;
        }

    private:
        void patchOperand(ShaderOperand& op,
                          std::vector<ShaderInstruction>& ret,
                          std::set<std::string>& initializedVariables)
        {
            if (op.type == ShaderOperand::Variable)
            {
                auto varName = std::get<std::string>(op.value);
                if (objects.find(varName) != objects.end())
                {
                    // Object defined by this scope
                    auto it = variables.find(varName);
                    if (it != variables.end())
                    {
                        // Object was promoted to variable
                        if (initializedVariables.find(varName) == initializedVariables.end())
                        {
                            // Variable uninitialized
                            // Create declaration/initialization instruction
                            if (it->second.second.type == ShaderOperand::None)
                            {
                                ret.emplace_back(
                                    ShaderInstructionFactory::declareVariable(varName, it->second.first)
                                );
                            }
                            else
                            {
                                auto initializer = it->second.second;
                                patchOperand(initializer, ret, initializedVariables);
                                ret.emplace_back(ShaderInstructionFactory::declareVariable(
                                        varName,
                                        it->second.first,
                                        initializer)
                                );
                            }
                            initializedVariables.insert(varName);
                        }
                    }
                    else
                    {
                        // Inline the object
                        op = objects.at(varName).second;
                        // Recursively patch the inlined operand
                        patchOperand(op, ret, initializedVariables);
                    }
                }
                else
                {
                    // Object defined in parent scope, we let the parent scope handle inline handling
                    // in its buildInstructionStream* call
                }
            }
            else if (op.type == ShaderOperand::Instruction)
            {
                auto& inst = std::get<ShaderInstruction>(op.value);
                for (auto& childOp : inst.operands)
                {
                    patchOperand(childOp, ret, initializedVariables);
                }
            }
        }

        void collectVariableReferencesInOperand(const ShaderOperand& op,
                                                std::set<std::string>& refs) const
        {
            if (op.type == ShaderOperand::Variable)
            {
                auto varName = std::get<std::string>(op.value);
                if (variables.find(varName) != variables.end())
                {
                    refs.insert(varName);
                }
            }
            else if (op.type == ShaderOperand::Instruction)
            {
                auto& inst = std::get<ShaderInstruction>(op.value);
                for (auto& childOp : inst.operands)
                {
                    collectVariableReferencesInOperand(childOp, refs);
                }
            }
        }

        void collectVariableReferences(const std::vector<ShaderInstruction>& insts,
                                       std::set<std::string>& refs) const
        {
            for (auto& inst : insts)
            {
                for (auto& op : inst.operands)
                {
                    collectVariableReferencesInOperand(op, refs);
                }
                // Recurse into nested branches/loops
                if (inst.code == ShaderInstruction::Branch)
                {
                    auto& trueBranch = std::get<std::vector<ShaderInstruction>>(inst.data.at(0));
                    auto& falseBranch = std::get<std::vector<ShaderInstruction>>(inst.data.at(1));
                    collectVariableReferences(trueBranch, refs);
                    collectVariableReferences(falseBranch, refs);
                }
                else if (inst.code == ShaderInstruction::Loop)
                {
                    auto& body = std::get<std::vector<ShaderInstruction>>(inst.data.at(0));
                    collectVariableReferences(body, refs);
                }
            }
        }

        void emitPendingDeclarations(const std::set<std::string>& referencedVars,
                                     std::vector<ShaderInstruction>& ret,
                                     std::set<std::string>& initializedVariables)
        {
            for (auto& varName : referencedVars)
            {
                if (initializedVariables.find(varName) != initializedVariables.end())
                    continue;
                auto it = variables.find(varName);
                if (it == variables.end())
                    continue;
                if (it->second.second.type == ShaderOperand::None)
                {
                    ret.emplace_back(
                        ShaderInstructionFactory::declareVariable(varName, it->second.first)
                    );
                }
                else
                {
                    auto initializer = it->second.second;
                    patchOperand(initializer, ret, initializedVariables);
                    ret.emplace_back(ShaderInstructionFactory::declareVariable(
                            varName,
                            it->second.first,
                            initializer)
                    );
                }
                initializedVariables.insert(varName);
            }
        }

    protected:
        void countReferencesInOperand(const ShaderOperand& op,
                                      std::map<std::string, int>& refCounts) const
        {
            if (op.type == ShaderOperand::Variable)
            {
                auto varName = std::get<std::string>(op.value);
                if (objects.find(varName) != objects.end())
                {
                    refCounts[varName]++;
                }
            }
            else if (op.type == ShaderOperand::Instruction)
            {
                auto& inst = std::get<ShaderInstruction>(op.value);
                for (auto& childOp : inst.operands)
                {
                    countReferencesInOperand(childOp, refCounts);
                }
            }
        }

        void countReferencesInInstructions(const std::vector<ShaderInstruction>& instrs,
                                           std::map<std::string, int>& refCounts) const
        {
            for (auto& inst : instrs)
            {
                for (auto& op : inst.operands)
                {
                    countReferencesInOperand(op, refCounts);
                }
                switch (inst.code)
                {
                case ShaderInstruction::Branch:
                    {
                        auto& trueBranch = std::get<std::vector<ShaderInstruction>>(inst.data.at(0));
                        countReferencesInInstructions(trueBranch, refCounts);
                        auto& falseBranch = std::get<std::vector<ShaderInstruction>>(inst.data.at(1));
                        countReferencesInInstructions(falseBranch, refCounts);
                        break;
                    }
                case ShaderInstruction::Loop:
                    {
                        auto& body = std::get<std::vector<ShaderInstruction>>(inst.data.at(0));
                        countReferencesInInstructions(body, refCounts);
                        break;
                    }
                default:
                    break;
                }
            }
        }

        void promoteMultiReferencedObjects()
        {
            std::map<std::string, int> refCounts;

            // Count references in the instruction stream
            countReferencesInInstructions(instructions, refCounts);

            // Count references in object initializers
            for (auto& [name, obj] : objects)
            {
                countReferencesInOperand(obj.second, refCounts);
            }

            // Promote objects referenced more than once
            for (auto& [varName, count] : refCounts)
            {
                if (count > 1 && variables.find(varName) == variables.end())
                {
                    variables.insert({varName, objects.at(varName)});
                }
            }
        }

        void promoteVariable(const std::string& targetName)
        {
            if (objects.find(targetName) != objects.end())
            {
                // Object defined at this scope level
                if (variables.find(targetName) == variables.end())
                {
                    variables.insert({targetName, objects.at(targetName)});
                }
            }
            else
            {
                bool found = false;
                // Search for object in parent scopes
                for (auto& scope : getStack())
                {
                    if (scope->objects.find(targetName) != scope->objects.end())
                    {
                        // Found the scope declaring the object
                        if (scope->variables.find(targetName) == scope->variables.end())
                        {
                            scope->variables.insert({targetName, scope->objects.at(targetName)});
                        }
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    throw std::runtime_error("Invalid object in instruction stream.");
                }
            }
        }

        void promoteVariablesInOperand(const ShaderOperand& op)
        {
            if (op.type == ShaderOperand::Variable)
            {
                promoteVariable(std::get<std::string>(op.value));
            }
            else if (op.type == ShaderOperand::Instruction)
            {
                auto& inst = std::get<ShaderInstruction>(op.value);
                for (auto& childOp : inst.operands)
                {
                    promoteVariablesInOperand(childOp);
                }
            }
        }

        std::map<std::string, std::pair<ShaderDataType, ShaderOperand>> objects;
        std::map<std::string, std::pair<ShaderDataType, ShaderOperand>> variables;
    public:
        std::vector<ShaderInstruction> instructions{};
    };
}

#endif //XENGINE_SHADERBLOCKSCOPE_HPP
