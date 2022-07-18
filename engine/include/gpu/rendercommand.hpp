/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_RENDERCOMMAND_HPP
#define XENGINE_RENDERCOMMAND_HPP

#include <vector>
#include <memory>

#include "gpu/vertexbuffer.hpp"
#include "gpu/texturebuffer.hpp"
#include "gpu/shaderbuffer.hpp"
#include "gpu/shaderprogram.hpp"
#include "gpu/shaderbinding.hpp"

namespace xng {
    /**
     * A render command
     */
    struct XENGINE_EXPORT RenderCommand {
        RenderCommand() = default;

        RenderCommand(VertexBuffer &vertexBuffer, std::vector<ShaderBinding> bindings)
                : vertexBuffer(&vertexBuffer), bindings(std::move(bindings)) {}

        VertexBuffer &getVertexBuffer() const {
            if (vertexBuffer == nullptr)
                throw std::runtime_error("Pass not initialized");
            return *vertexBuffer;
        }

        const std::vector<ShaderBinding> &getBindings() const {
            if (vertexBuffer == nullptr)
                throw std::runtime_error("Pass not initialized");
            return bindings;
        }

    private:
        VertexBuffer *vertexBuffer = nullptr;
        std::vector<ShaderBinding> bindings;
    };
}

#endif //XENGINE_RENDERCOMMAND_HPP
