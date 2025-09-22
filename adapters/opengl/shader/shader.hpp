/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_SHADER_HPP
#define XENGINE_SHADER_HPP

#include <utility>

#include "shader/shadersource.hpp"

#include "xng/resource/resource.hpp"

namespace xng {
    struct XENGINE_EXPORT Shader : public Resource {
        Shader() = default;

        Shader(ShaderSource vertexShader,
               ShaderSource fragmentShader,
               ShaderSource geometryShader)
                : vertexShader(std::move(vertexShader)),
                geometryShader(std::move(geometryShader)),
                fragmentShader(std::move(fragmentShader)) {}

        ~Shader() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Shader>(*this);
        }

        std::type_index getTypeIndex() const override {
            return typeid(Shader);
        }

        ShaderSource vertexShader;
        ShaderSource geometryShader;
        ShaderSource fragmentShader;
    };
}
#endif //XENGINE_SHADER_HPP
