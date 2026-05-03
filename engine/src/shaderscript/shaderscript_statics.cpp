/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "xng/shaderscript/blockscope.hpp"
#include "xng/shaderscript/shaderscope.hpp"
#include "xng/shaderscript/functionscope.hpp"

// TODO: Redesign DSL to do deferred reverse instruction recording in outputs such as Attribute assignment or function return which will make the thread_local usage unnecessary.

namespace xng::ShaderScript {
    std::vector<BlockScope*>& BlockScope::getStack() {
        thread_local std::vector<BlockScope*> stack;
        return stack;
    }

    BlockScope*& BlockScope::getCurrent() {
        thread_local BlockScope* current = nullptr;
        return current;
    }

    int& BlockScope::getObjectCounter()
    {
        thread_local int objectCounter;
        return objectCounter;
    }

    ShaderScope*& ShaderScope::getCurrent() {
        thread_local ShaderScope* current = nullptr;
        return current;
    }

    std::vector<ShaderOperand>& IRBaseParam::getArgumentValuesStorage() {
        thread_local std::vector<ShaderOperand> argumentValues;
        return argumentValues;
    }

    std::vector<ShaderFunction::Argument>& IRBaseParam::getArgumentsStorage() {
        thread_local std::vector<ShaderFunction::Argument> arguments;
        return arguments;
    }

    std::vector<FunctionScope::Context>& FunctionScope::getStack() {
        thread_local std::vector<FunctionScope::Context> stack;
        return stack;
    }
}
