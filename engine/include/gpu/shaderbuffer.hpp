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

#ifndef XENGINE_SHADERBUFFER_HPP
#define XENGINE_SHADERBUFFER_HPP

#include "gpu/renderbuffer.hpp"
#include "gpu/gpufence.hpp"

#include "shaderbufferdesc.hpp"

namespace xng {
    class ShaderBuffer : public RenderBuffer {
    public:
        ~ShaderBuffer() override = default;

        Type getType() override {
            return SHADER_BUFFER;
        }

        virtual const ShaderBufferDesc &getDescription() = 0;

        RenderBufferType getBufferType() override {
            return getDescription().bufferType;
        }

        /**
         * Upload the given data to the shader buffer,
         *
         * @param data
         * @param size must match the size specified at creation of the shader buffer
         */
        virtual std::unique_ptr<GpuFence> upload(const uint8_t *data, size_t size) = 0;

        template<typename T>
        std::unique_ptr<GpuFence> upload(const T &data) {
            return upload(reinterpret_cast<const uint8_t *>(&data), sizeof(T));
        }
    };
}
#endif //XENGINE_SHADERBUFFER_HPP
