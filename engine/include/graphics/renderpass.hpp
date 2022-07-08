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

#ifndef XENGINE_RENDERPASS_HPP
#define XENGINE_RENDERPASS_HPP

#include <vector>
#include <memory>

#include "graphics/vertexbuffer.hpp"
#include "graphics/texturebuffer.hpp"
#include "shaderbuffer.hpp"
#include "shaderprogram.hpp"

namespace xng {
    struct XENGINE_EXPORT RenderPass {
        enum BindingType {
            TEXTURE_BUFFER,
            SHADER_BUFFER
        };

        /**
         * A shader binding is the data accessed from a shader via explicit binding. (eg. layout(binding = X) in GLSL)
         */
        struct ShaderBinding {
            ShaderBinding() = default;

            explicit ShaderBinding(TextureBuffer &textureBuffer) {
                type = TEXTURE_BUFFER;
                object = &textureBuffer;
            }

            explicit ShaderBinding(ShaderBuffer &shaderBuffer) {
                type = SHADER_BUFFER;
                object = &shaderBuffer;
            }

            ShaderBinding(const ShaderBinding &other) = default;

            ShaderBinding(ShaderBinding &&other) = default;

            ShaderBinding &operator=(ShaderBinding &&other) = default;

            ShaderBinding &operator=(const ShaderBinding &other) = default;

            BindingType getType() const {
                if (object == nullptr)
                    throw std::runtime_error("Binding not initialized");
                return type;
            }

            TextureBuffer &getTextureBuffer() const {
                if (object == nullptr)
                    throw std::runtime_error("Binding not initialized");
                return dynamic_cast<TextureBuffer &>(*object);
            }

            ShaderBuffer &getShaderBuffer() const {
                if (object == nullptr)
                    throw std::runtime_error("Binding not initialized");
                return dynamic_cast<ShaderBuffer &>(*object);
            }

        private:
            BindingType type;
            RenderObject *object = nullptr;
        };

        RenderPass() = default;

        RenderPass(VertexBuffer &vertexBuffer, std::vector<ShaderBinding> bindings)
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

#endif //XENGINE_RENDERPASS_HPP
