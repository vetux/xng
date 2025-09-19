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

#ifndef XENGINE_NODEMATRIXSUBSCRIPT_HPP
#define XENGINE_NODEMATRIXSUBSCRIPT_HPP

#include "xng/rendergraph/shader/shadernode.hpp"

namespace xng {
    struct NodeMatrixSubscript final : ShaderNode {
        std::unique_ptr<ShaderNode> matrix;
        std::unique_ptr<ShaderNode> column;
        std::unique_ptr<ShaderNode> row;

        NodeMatrixSubscript(std::unique_ptr<ShaderNode> matrix,
                            std::unique_ptr<ShaderNode> column,
                            std::unique_ptr<ShaderNode> row)
            : matrix(std::move(matrix)),
              column(std::move(column)),
              row(std::move(row)) {
        }

        NodeType getType() const override {
            return SUBSCRIPT_MATRIX;
        }

        std::unique_ptr<ShaderNode> copy() const override {
            return std::make_unique<NodeMatrixSubscript>(matrix->copy(), column->copy(), row ? row->copy() : nullptr);
        }
    };
}

#endif //XENGINE_NODEMATRIXSUBSCRIPT_HPP
