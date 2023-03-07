/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_TEXTUREARRAYBUFFERDESC_HPP
#define XENGINE_TEXTUREARRAYBUFFERDESC_HPP


#include "xng/math/vector2.hpp"

#include "xng/gpu/textureproperties.hpp"
#include "xng/gpu/renderbuffertype.hpp"
#include "xng/gpu/texturebufferdesc.hpp"

#include "xng/util/hashcombine.hpp"

#include "xng/io/messageable.hpp"

#include "xng/io/message.hpp"

namespace xng {
    struct TextureArrayBufferDesc : public Messageable {
        size_t textureCount{}; // The number of textures in the array
        TextureBufferDesc textureDesc{}; // The format of the textures

        bool operator==(const TextureArrayBufferDesc &other) const {
            return textureCount == other.textureCount
                   && textureDesc.size == other.textureDesc.size
                   && textureDesc.samples == other.textureDesc.samples
                   && textureDesc.textureType == other.textureDesc.textureType
                   && textureDesc.format == other.textureDesc.format
                   && textureDesc.wrapping == other.textureDesc.wrapping
                   && textureDesc.filterMin == other.textureDesc.filterMin
                   && textureDesc.filterMag == other.textureDesc.filterMag
                   && textureDesc.generateMipmap == other.textureDesc.generateMipmap
                   && textureDesc.mipmapFilter == other.textureDesc.mipmapFilter
                   && textureDesc.fixedSampleLocations == other.textureDesc.fixedSampleLocations
                   && textureDesc.bufferType == other.textureDesc.bufferType;
        }

        Messageable &operator<<(const Message &message) override {
            message.value("textureCount", textureCount, static_cast<size_t>(0));
            message.value("textureDesc", textureDesc);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            textureCount >> message["textureCount"];
            textureDesc >> message["textureDesc"];
            return message;
        }
    };
}

namespace std {
    template<>
    struct hash<xng::TextureArrayBufferDesc> {
        std::size_t operator()(const xng::TextureArrayBufferDesc &k) const {
            size_t ret = 0;
            xng::hash_combine(ret, k.textureCount);
            xng::hash_combine(ret, k.textureDesc);
            return ret;
        }
    };
}

#endif //XENGINE_TEXTUREARRAYBUFFERDESC_HPP
