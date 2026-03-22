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

#ifndef XENGINE_FUNCTIONSCOPE_HPP
#define XENGINE_FUNCTIONSCOPE_HPP

#include "xng/rendergraph/shader/shaderfunction.hpp"
#include "xng/rendergraph/shader/shaderinstructionfactory.hpp"

#include "xng/shaderscript/blockscope.hpp"
#include "xng/shaderscript/shaderobject.hpp"

namespace xng::ShaderScript
{
    class XENGINE_EXPORT IRBaseParam
    {
    public:
        static const std::vector<ShaderFunction::Argument>& getArguments() { return getArgumentsStorage(); }

        static std::vector<ShaderOperand> getArgumentValues() { return getArgumentValuesStorage(); }

        static void clear()
        {
            getArgumentValuesStorage().clear();
            getArgumentsStorage().clear();
        }

    protected:
        static std::vector<ShaderOperand>& getArgumentValuesStorage();
        static std::vector<ShaderFunction::Argument>& getArgumentsStorage();
    };

    /**
     * Optional Argument wrapper to simplify argument registration.
     *
     * Removes the need to call createArgument on the function scope by using the wrapper in the
     * function signature and allows the helper macros to get the return values for the Call invocation.
     *
     * E.g. vec2 myFunction(Param<vec2> myarg);
     *
     * This assumes consistent argument initialization order across call sites which should be the case on current
     * mainstream compilers (x86 cdecl is right-to-left across all call sites, ARM is left-to-right across all call sites)
     *
     * @tparam T
     */
    template <typename T>
    class Param : public IRBaseParam
    {
    public:
        Param(Param &other)
        {
            const auto argName = "arg" + std::to_string(getArgumentsStorage().size());
            val = std::make_unique<T>(ShaderOperand::argument(argName));
            getArgumentsStorage().emplace_back(ShaderFunction::Argument(T::TYPE, argName));
            getArgumentValuesStorage().emplace_back(other.value().operand);
        }

        Param(ShaderObject argumentValue)
        {
            const auto argName = "arg" + std::to_string(getArgumentsStorage().size());
            val = std::make_unique<T>(ShaderOperand::argument(argName));
            getArgumentsStorage().emplace_back(ShaderFunction::Argument(T::TYPE, argName));
            getArgumentValuesStorage().emplace_back(argumentValue.operand);
        }

        Param(T argumentValue)
        {
            const auto argName = "arg" + std::to_string(getArgumentsStorage().size());
            val = std::make_unique<T>(ShaderOperand::argument(argName));
            getArgumentsStorage().emplace_back(ShaderFunction::Argument(T::TYPE, argName));
            getArgumentValuesStorage().emplace_back(static_cast<ShaderObject>(argumentValue).operand);
        }

        T& value()
        {
            return *val;
        }

        explicit operator const ShaderObject&() const { return *val; }

        operator const T&() const { return *val; }

    private:
        std::unique_ptr<T> val;
    };

    class XENGINE_EXPORT FunctionScope
    {
    public:
        struct Context
        {
            std::vector<BlockScope*> stack;
            BlockScope* current;
        };

        explicit FunctionScope(std::string name)
            : name(std::move(name))
        {
            // Save current block scope state for nested ShaderFunctionScopes
            Context c = {BlockScope::getStack(), BlockScope::getCurrent()};
            getStack().push_back(c);
            BlockScope::getStack() = {};
            BlockScope::getCurrent() = nullptr;
            block = std::make_unique<BlockScope>();

            arguments = IRBaseParam::getArguments();
            argumentValues = IRBaseParam::getArgumentValues();
            IRBaseParam::clear();
        }

        ~FunctionScope()
        {
            assert(BlockScope::getStack().empty());

            block = nullptr;

            // Restore block scope state
            Context c = getStack().back();
            getStack().pop_back();
            BlockScope::getStack() = c.stack;
            BlockScope::getCurrent() = c.current;
        }

        template <typename T>
        T addArgument(const std::string& name)
        {
            if (returnType.has_value())
            {
                throw std::runtime_error("Attempted to modify function with existing return value");
            }
            arguments.emplace_back(ShaderFunction::Argument(T::TYPE, name));
            return T(ShaderOperand::argument(name));
        }

        template <typename T>
        void setReturnType(const T&)
        {
            if (returnType.has_value())
            {
                if (returnType.value() != T::TYPE)
                {
                    throw std::runtime_error("Attempted to change return type");
                }
            }
            else
            {
                returnType = T::TYPE;
            }
        }

        ShaderFunction build()
        {
            if (returnType.has_value())
                return {name, arguments, block->buildInstructionStream(), returnType.value()};
            return {name, arguments, block->buildInstructionStream()};
        }

        const std::vector<ShaderOperand>& getArgumentValues() { return argumentValues; }

    private:
        std::string name{};

        std::vector<ShaderFunction::Argument> arguments{};
        std::optional<ShaderDataType> returnType{};

        std::vector<ShaderOperand> argumentValues;

        std::unique_ptr<BlockScope> block;

        static std::vector<Context>& getStack();
    };
}

#endif //XENGINE_FUNCTIONSCOPE_HPP
