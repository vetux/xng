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

#ifndef XENGINE_SHADERBINDING_HPP
#define XENGINE_SHADERBINDING_HPP

#include "gpu/renderdevice.hpp"

namespace xng {
    /**
     * A shader binding is the data accessed from a shader via explicit binding. (eg. layout(binding = X) in GLSL)
     */
    struct ShaderBinding {
        enum BindingType {
            BINDING_TEXTURE_BUFFER,
            BINDING_SHADER_BUFFER
        };

        ShaderBinding() = default;

        explicit ShaderBinding(TextureBuffer &textureBuffer) {
            type = BINDING_TEXTURE_BUFFER;
            object = &textureBuffer;
        }

        explicit ShaderBinding(ShaderBuffer &shaderBuffer) {
            type = BINDING_SHADER_BUFFER;
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
}

#endif //XENGINE_SHADERBINDING_HPP
