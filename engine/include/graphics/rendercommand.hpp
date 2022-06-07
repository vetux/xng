/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_RENDERCOMMAND_HPP
#define XENGINE_RENDERCOMMAND_HPP

#include <vector>
#include <memory>

#include "graphics/meshbuffer.hpp"
#include "graphics/texturebuffer.hpp"
#include "graphics/shaderbuffer.hpp"
#include "graphics/shaderprogram.hpp"

namespace xengine {
    struct XENGINE_EXPORT RenderCommand {
        explicit RenderCommand(MeshBufferView &mesh) : mesh(&mesh) {}

        RenderCommand(const RenderCommand &other) = default;

        RenderCommand(RenderCommand &&other) = default;

        RenderCommand &operator=(RenderCommand &&other) = default;

        enum BindingType {
            TEXTURE_BUFFER,
            SHADER_BUFFER
        };

        struct Binding {
            BindingType type;
            RenderObject *object;

            explicit Binding(TextureBufferView &textureBuffer) {
                type = TEXTURE_BUFFER;
                object = &textureBuffer;
            }

            explicit Binding(ShaderBuffer &shaderBuffer) {
                type = SHADER_BUFFER;
                object = &shaderBuffer;
            }

            TextureBufferView &getTextureBuffer() { return dynamic_cast<TextureBufferView &>(*object); }

            ShaderBuffer &getShaderBuffer() { return dynamic_cast<ShaderBuffer &>(*object); }
        };

        MeshBufferView *mesh;
        std::vector<Binding> shaderBindings; // The bindings accessible to the shader in the pipeline in order.
    };
}

#endif //XENGINE_RENDERCOMMAND_HPP
