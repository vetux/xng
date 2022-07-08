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

#include "render/graph/gbuffer.hpp"

namespace xng {
    GBuffer::GBuffer(RenderDevice &device, Vec2i size, int samples)
            : size(size), samples(samples) {
        if (size.x < 1 || size.y < 1)
            throw std::runtime_error("Invalid size");
        if (samples < 1)
            throw std::runtime_error("Invalid samples");

        TextureBufferDesc attr;
        attr.textureType = TEXTURE_2D_MULTISAMPLE;
        attr.size = size;
        attr.samples = samples;

        for (int i = GEOMETRY_TEXTURE_BEGIN, index = 0; i < GEOMETRY_TEXTURE_END; i++, index++) {
            auto tex = static_cast<GTexture>(i);
            if (i < DIFFUSE) {
                attr.format = RGBA32F;
            } else if (i < ID_SHININESS) {
                attr.format = RGBA;
            } else {
                attr.format = RGBA32I;
            }
            textures[tex] = device.createTextureBuffer(attr);
        }

        attr.format = DEPTH_STENCIL;

        textures[DEPTH] = device.createTextureBuffer(attr);
    }

    GBuffer::~GBuffer() = default;

    Vec2i GBuffer::getSize() const {
        return size;
    }

    int GBuffer::getSamples() const {
        return samples;
    }

    TextureBuffer &GBuffer::getTexture(GBuffer::GTexture type) const {
        return *textures.at(type);
    }

    void GBuffer::attachTextures(RenderTarget &target) const {
        std::vector<TextureBuffer *> tex;
        for (int i = GBuffer::GEOMETRY_TEXTURE_BEGIN; i < GBuffer::GEOMETRY_TEXTURE_END; i++) {
            tex.emplace_back(&getTexture((GTexture)i));
        }
        target.setColorAttachments(tex);
        target.setDepthStencilAttachment(&getTexture(DEPTH));
    }

    void GBuffer::detachTextures(RenderTarget &target) const {
        target.setColorAttachments(std::vector<TextureBuffer*>());
        target.setDepthStencilAttachment(nullptr);
    }
}