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

#include "render/deferred/gbuffer.hpp"

namespace xengine {
    GBuffer::GBuffer(RenderAllocator &allocator, Vec2i size, int samples)
            : renderAllocator(allocator), size(size), samples(samples) {
        if (size.x < 1 || size.y < 1)
            throw std::runtime_error("Invalid size");
        if (samples < 1)
            throw std::runtime_error("Invalid samples");

        renderTarget = allocator.createRenderTarget(size, samples);
        passTarget = allocator.createRenderTarget(size);
        screenQuad = allocator.createMeshBuffer(Mesh::normalizedQuad());

        TextureBuffer::Attributes attr;
        attr.textureType = TextureBuffer::TEXTURE_2D_MULTISAMPLE;
        attr.size = size;
        attr.samples = samples;

        for (int i = GEOMETRY_TEXTURE_BEGIN, index = 0; i < GEOMETRY_TEXTURE_END; i++, index++) {
            auto tex = static_cast<GTexture>(i);
            if (i < DIFFUSE) {
                attr.format = TextureBuffer::RGBA32F;
            } else if (i < ID_SHININESS) {
                attr.format = TextureBuffer::RGBA;
            } else {
                attr.format = TextureBuffer::RGBA32I;
            }
            textures[tex] = renderAllocator.createTextureBuffer(attr);
            renderTarget->attachColor(index, *textures.at(tex));
        }

        attr.textureType = TextureBuffer::TEXTURE_2D;
        attr.format = TextureBuffer::DEPTH_STENCIL;

        textures[DEPTH] = renderAllocator.createTextureBuffer(attr);
    }

    GBuffer::~GBuffer() {
        renderTarget->detachDepthStencil();
    }

    void GBuffer::setSize(const Vec2i &s) {
        if (size == s)
            return;
        if (s.x < 1 || s.y < 1)
            throw std::runtime_error("Invalid size");
        size = s;
        reallocateObjects();
    }

    Vec2i GBuffer::getSize() {
        return size;
    }

    void GBuffer::setSamples(int value) {
        if (samples == value)
            return;
        if (value < 1)
            throw std::runtime_error("Invalid samples");
        samples = value;
        reallocateObjects();
    }

    int GBuffer::getSamples() {
        return samples;
    }

    TextureBuffer &GBuffer::getTexture(GTexture type) {
        return *textures.at(type);
    }

    RenderTarget &GBuffer::getRenderTarget() {
        assert(renderTarget != nullptr);
        return *renderTarget;
    }

    MeshBuffer &GBuffer::getScreenQuad() {
        assert(screenQuad != nullptr);
        return *screenQuad;
    }

    RenderTarget &GBuffer::getPassTarget() {
        return *passTarget;
    }

    void GBuffer::reallocateObjects() {
        //Detach textures
        for (int i = 0; i < GEOMETRY_TEXTURE_END; i++)
            renderTarget->detachColor(i);
        renderTarget->detachDepthStencil();

        renderTarget = renderAllocator.createRenderTarget(size, samples);
        renderTarget->setNumberOfColorAttachments(static_cast<int>(GEOMETRY_TEXTURE_END));

        TextureBuffer::Attributes attr;
        attr.textureType = TextureBuffer::TEXTURE_2D_MULTISAMPLE;
        attr.size = size;
        attr.samples = samples;
        attr.fixedSampleLocations = true;

        for (int i = GEOMETRY_TEXTURE_BEGIN, index = 0; i < GEOMETRY_TEXTURE_END; i++, index++) {
            auto tex = static_cast<GTexture>(i);
            if (i < DIFFUSE) {
                attr.format = TextureBuffer::RGBA32F;
            } else if (i < ID_SHININESS) {
                attr.format = TextureBuffer::RGBA;
            } else {
                attr.format = TextureBuffer::RGBA32I;
            }
            textures[tex] = renderAllocator.createTextureBuffer(attr);
            renderTarget->attachColor(index, *textures.at(tex));
        }

        attr.format = TextureBuffer::DEPTH_STENCIL;

        textures[DEPTH] = renderAllocator.createTextureBuffer(attr);
        renderTarget->attachDepthStencil(*textures.at(DEPTH));

        passTarget = renderAllocator.createRenderTarget(size);
    }
}