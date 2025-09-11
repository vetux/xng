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

#ifndef XENGINE_FGLOOPBUILDER_HPP
#define XENGINE_FGLOOPBUILDER_HPP

#include "xng/render/graph2/shader/fgshaderbuilder.hpp"

namespace xng {
    class FGLoopBuilder {
    public:
        /**
         * Helper function for creating a while loop which loops until the condition evaluates to false.
         *
         * @param condition
         */
        void loopWhile(const std::unique_ptr<FGShaderNode> &condition) {
            loopType = WHILE;
            loopPredicate = condition->copy();
            loopInitializer = nullptr;
            loopIterator = nullptr;
            body.clear();
        }

        void loopFor(const std::unique_ptr<FGShaderNode> &initializer,
                     const std::unique_ptr<FGShaderNode> &predicate,
                     const std::unique_ptr<FGShaderNode> &increment) {
            loopType = FOR;
            loopInitializer = initializer->copy();
            loopPredicate = predicate->copy();
            loopIterator = increment->copy();
            body.clear();
        }

        /**
         * Helper function for creating a loop by specifying an iterator variable name of type int
         * which will be initialized to the value in iterator start and loop until iterator end using the specified step.
         *
         * @param variableName
         * @param start
         * @param end
         * @param step
         */
        void loopFor(const std::string &variableName,
                     const std::unique_ptr<FGShaderNode> &start,
                     const std::unique_ptr<FGShaderNode> &end,
                     const int step = 1) {
            loopType = FOR_WITH_VARIABLE;

            iteratorVariable = variableName;
            iteratorStart = start->copy();
            iteratorEnd = end->copy();
            iteratorStep = step;

            loopInitializer = nullptr;
            loopPredicate = nullptr;
            loopIterator = nullptr;
            body.clear();
        }

        void add(const std::unique_ptr<FGShaderNode> &node) {
            body.push_back(node->copy());
        }

        void add(const std::vector<std::unique_ptr<FGShaderNode> > &nodes) {
            for (auto &node: nodes) {
                body.push_back(node->copy());
            }
        }

        void endLoop() {
        }

        std::vector<std::unique_ptr<FGShaderNode> > build(FGShaderBuilder &builder) {
            std::vector<std::unique_ptr<FGShaderNode> > result;
            if (loopType == FOR_WITH_VARIABLE) {
                loopInitializer = (builder.createVariable(iteratorVariable,
                                                          FGShaderValue(FGShaderValue::SCALAR,
                                                                        FGShaderValue::SIGNED_INT),
                                                          iteratorStart));
                loopPredicate = builder.compareLess(builder.variable(iteratorVariable), iteratorEnd);
                loopIterator = builder.assignVariable(iteratorVariable,
                                                      builder.add(builder.variable(iteratorVariable),
                                                                  builder.literal(iteratorStep)));
            }
            result.emplace_back(builder.loop(loopInitializer, loopPredicate, loopIterator, body));
            return result;
        }

    private:
        enum LoopType {
            NONE,
            WHILE,
            FOR,
            FOR_WITH_VARIABLE
        } loopType = NONE;

        std::unique_ptr<FGShaderNode> loopInitializer;
        std::unique_ptr<FGShaderNode> loopPredicate;
        std::unique_ptr<FGShaderNode> loopIterator;
        std::vector<std::unique_ptr<FGShaderNode> > body;

        std::string iteratorVariable;
        std::unique_ptr<FGShaderNode> iteratorStart;
        std::unique_ptr<FGShaderNode> iteratorEnd;
        int iteratorStep{};
    };
}
#endif //XENGINE_FGLOOPBUILDER_HPP
