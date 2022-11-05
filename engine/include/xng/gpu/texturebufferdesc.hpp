/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_TEXTUREBUFFERDESC_HPP
#define XENGINE_TEXTUREBUFFERDESC_HPP

#include "xng/math/vector2.hpp"

#include "textureproperties.hpp"
#include "renderbuffertype.hpp"

#include "xng/util/hashcombine.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    struct TextureBufferDesc : public Messageable {
        Vec2i size = {1, 1};
        int samples = 1; //Ignored if texture is not TEXTURE_2D_MULTISAMPLE
        TextureType textureType = TEXTURE_2D;
        ColorFormat format = RGBA;
        TextureWrapping wrapping = CLAMP_TO_BORDER;
        TextureFiltering filterMin = NEAREST;
        TextureFiltering filterMag = NEAREST;
        bool generateMipmap = false;
        MipMapFiltering mipmapFilter = NEAREST_MIPMAP_NEAREST;
        bool fixedSampleLocations = false;
        RenderBufferType bufferType = HOST_VISIBLE;

        bool operator==(const TextureBufferDesc &other) const {
            return size == other.size
                   && samples == other.samples
                   && textureType == other.textureType
                   && format == other.format
                   && wrapping == other.wrapping
                   && filterMin == other.filterMin
                   && filterMag == other.filterMag
                   && generateMipmap == other.generateMipmap
                   && mipmapFilter == other.mipmapFilter
                   && fixedSampleLocations == other.fixedSampleLocations
                   && bufferType == other.bufferType;
        }

        Messageable &operator<<(const Message &message) override {
            size << message.value("size");
            samples = message.value("samples", 0);
            textureType = (TextureType) message.value("textureType", (int) TEXTURE_2D);
            format = (ColorFormat) message.value("format", (int) RGBA);
            wrapping = (TextureWrapping) message.value("wrapping", (int)CLAMP_TO_BORDER);
            filterMin = (TextureFiltering) message.value("filterMin", (int)NEAREST);
            filterMag = (TextureFiltering) message.value("filterMax", (int)NEAREST);
            generateMipmap = message.value("generateMipmap", false);
            mipmapFilter = (MipMapFiltering)message.value("mipmapFilter", (int)NEAREST_MIPMAP_NEAREST);
            fixedSampleLocations = message.value("fixedSampleLocations", false);
            bufferType = (RenderBufferType)message.value("bufferType", (int)(HOST_VISIBLE));
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            size >> message["size"];
            message["samples"] = samples;
            message["textureType"] = (int)textureType;
            message["format"] = (int)format;
            message["wrapping"] = (int)wrapping;
            message["filterMin"] = (int)filterMin;
            message["filterMag"] = (int)filterMag;
            message["generateMipmap"] = (int)generateMipmap;
            message["mipmapFilter"] = (int)mipmapFilter;
            message["fixedSampleLocations"] = fixedSampleLocations;
            message["bufferType"] = (int) bufferType;
            return message;
        }
    };
}

using namespace xng;
namespace std {
    template<>
    struct hash<TextureBufferDesc> {
        std::size_t operator()(const TextureBufferDesc &k) const {
            size_t ret = 0;
            hash_combine(ret, k.size.x);
            hash_combine(ret, k.size.y);
            hash_combine(ret, k.samples);
            hash_combine(ret, k.textureType);
            hash_combine(ret, k.format);
            hash_combine(ret, k.wrapping);
            hash_combine(ret, k.filterMin);
            hash_combine(ret, k.filterMag);
            hash_combine(ret, k.generateMipmap);
            hash_combine(ret, k.mipmapFilter);
            hash_combine(ret, k.fixedSampleLocations);
            hash_combine(ret, k.bufferType);
            return ret;
        }
    };
}

#endif //XENGINE_TEXTUREBUFFERDESC_HPP
