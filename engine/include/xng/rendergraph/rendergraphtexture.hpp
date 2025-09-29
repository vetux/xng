/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_RENDERGRAPHTEXTURE_HPP
#define XENGINE_RENDERGRAPHTEXTURE_HPP

#include "xng/math/vector2.hpp"

#include "xng/rendergraph/rendergraphtextureproperties.hpp"

#include "xng/graphics/color.hpp"

namespace xng {
    struct RenderGraphTexture final : Messageable {
        Vec2i size = {1, 1};
        TextureType textureType = TEXTURE_2D;
        ColorFormat format = RGBA;

        TextureWrapping wrapping = CLAMP_TO_BORDER;
        TextureFiltering filterMin = NEAREST;
        TextureFiltering filterMag = NEAREST;

        int samples = 1; //Ignored if texture is not TEXTURE_2D_MULTISAMPLE
        int mipMapLevels = 1;
        MipMapFiltering mipMapFilter = NEAREST_MIPMAP_LINEAR;
        bool fixedSampleLocations = false;

        ColorRGBA borderColor = ColorRGBA::black(1, 0);

        size_t arrayLayers = 0; // The number of layers in this array texture

        // Render textures can only be used as attachments and cannot be bound for read access by shaders.
        bool isRenderTexture = false;

        Messageable &operator<<(const Message &message) override {
            message.value("size", size);
            message.value("textureType", textureType);
            message.value("format", format);
            message.value("wrapping", wrapping);
            message.value("filterMin", filterMin);
            message.value("filterMag", filterMag);
            message.value("samples", samples);
            message.value("mipMapLevels", mipMapLevels);
            message.value("mipMapFilter", mipMapFilter);
            message.value("fixedSampleLocations", fixedSampleLocations);
            message.value("borderColor", borderColor);
            message.value("arrayLayers", arrayLayers);
            message.value("isRenderTexture", isRenderTexture);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            size >> message["size"];
            textureType >> message["textureType"];
            format >> message["format"];
            wrapping >> message["wrapping"];
            filterMin >> message["filterMin"];
            filterMag >> message["filterMag"];
            samples >> message["samples"];
            mipMapLevels >> message["mipMapLevels"];
            mipMapFilter >> message["mipMapFilter"];
            fixedSampleLocations >> message["fixedSampleLocations"];
            borderColor >> message["borderColor"];
            arrayLayers >> message["arrayLayers"];
            isRenderTexture >> message["isRenderTexture"];
            return message;
        }
    };
}

#endif //XENGINE_RENDERGRAPHTEXTURE_HPP
