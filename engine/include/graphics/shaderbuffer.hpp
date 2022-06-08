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

#ifndef XENGINE_SHADERBUFFER_HPP
#define XENGINE_SHADERBUFFER_HPP

#include "graphics/renderobject.hpp"

#include "graphics/shaderbufferdesc.hpp"

namespace xengine {
    class ShaderBuffer : public RenderObject {
    public:
        ~ShaderBuffer() override = default;

        virtual const ShaderBufferDesc &getDescription() = 0;

        /**
         * Upload the given data to the shader buffer,
         * size has to match the size of the shader buffer.
         *
         * @param data
         * @param size
         */
        virtual void upload(const uint8_t *data, size_t size) = 0;

        template<typename T>
        void upload(const T &data) {
            upload(reinterpret_cast<const uint8_t *>(&data), sizeof(T));
        }
    };
}
#endif //XENGINE_SHADERBUFFER_HPP
