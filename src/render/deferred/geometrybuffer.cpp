/**
 *  Mana - 3D Game Engine
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

#include "render/deferred/geometrybuffer.hpp"

namespace engine {
    GeometryBuffer::GeometryBuffer(RenderAllocator &allocator, Vec2i size, int samples)
            : renderAllocator(allocator), size(size), samples(samples) {
        if (size.x < 1 || size.y < 1)
            throw std::runtime_error("Invalid size");
        if (samples < 1)
            throw std::runtime_error("Invalid samples");

        renderTarget = allocator.createRenderTarget(size, samples);

        const Mesh quadMesh(Mesh::TRI,
                            {
                                    Vertex({-1, 1, 0}, {0, 1}),
                                    Vertex({1, 1, 0}, {1, 1}),
                                    Vertex({1, -1, 0}, {1, 0}),
                                    Vertex({-1, 1, 0}, {0, 1}),
                                    Vertex({1, -1, 0}, {1, 0}),
                                    Vertex({-1, -1, 0}, {0, 0})
                            });

        screenQuad = allocator.createMeshBuffer(quadMesh);
    }

    GeometryBuffer::~GeometryBuffer() {
        renderTarget->detachDepthStencil();
    }

    void GeometryBuffer::setSize(const Vec2i &s) {
        if (size == s)
            return;
        if (s.x < 1 || s.y < 1)
            throw std::runtime_error("Invalid size");
        size = s;
        reallocateBuffers();
    }

    Vec2i GeometryBuffer::getSize() {
        return size;
    }

    void GeometryBuffer::setSamples(int value) {
        if (samples == value)
            return;
        if (value < 1)
            throw std::runtime_error("Invalid samples");
        samples = value;
        reallocateBuffers();
    }

    int GeometryBuffer::getSamples() {
        return samples;
    }

    RenderTarget &GeometryBuffer::getRenderTarget() {
        assert(renderTarget != nullptr);
        return *renderTarget;
    }

    void GeometryBuffer::addBuffer(const std::string &name, TextureBuffer::ColorFormat format) {
        auto it = buffers.find(name);
        if (it != buffers.end()) {
            if (it->second->getAttributes().format != format)
                throw std::runtime_error("Buffer with different format already exists " + name);
            return;
        }

        TextureBuffer::Attributes attr;
        attr.textureType = TextureBuffer::TEXTURE_2D_MULTISAMPLE;
        attr.size = size;
        attr.format = format;
        buffers[name] = renderAllocator.createTextureBuffer(attr);
        formats[name] = format;
    }

    TextureBuffer &GeometryBuffer::getBuffer(const std::string &name) {
        return *buffers.at(name);
    }

    void GeometryBuffer::attachColor(const std::vector<std::string> &attachments) {
        detachColor();
        renderTarget->setNumberOfColorAttachments(static_cast<int>(attachments.size()));
        for (int i = 0; i < attachments.size(); i++)
            renderTarget->attachColor(i, *buffers.at(attachments.at(i)));
        currentColor = attachments;
    }

    void GeometryBuffer::detachColor() {
        for (int i = 0; i < currentColor.size(); i++)
            renderTarget->detachColor(i);
        currentColor.clear();
    }

    void GeometryBuffer::attachDepthStencil(const std::string &name) {
        detachDepthStencil();
        auto &buffer = getBuffer(name);
        if (buffer.getAttributes().format != TextureBuffer::ColorFormat::DEPTH_STENCIL)
            throw std::runtime_error("Invalid depth stencil texture " + name);
        renderTarget->detachDepthStencil();
        renderTarget->attachDepthStencil(buffer);
        currentDepthStencil = name;
    }

    void GeometryBuffer::detachDepthStencil() {
        renderTarget->detachDepthStencil();
        currentDepthStencil = "";
    }

    MeshBuffer &GeometryBuffer::getScreenQuad() {
        assert(screenQuad != nullptr);
        return *screenQuad;
    }

    void GeometryBuffer::reallocateBuffers() {
        //Detach textures
        for (int i = 0; i < currentColor.size(); i++)
            renderTarget->detachColor(i);

        renderTarget->detachDepthStencil();

        renderTarget = renderAllocator.createRenderTarget(size, samples);

        //Reallocate objects
        for (auto &buf: buffers) {
            TextureBuffer::Attributes attr;
            attr.textureType = TextureBuffer::TEXTURE_2D_MULTISAMPLE;
            attr.size = size;
            attr.format = formats.at(buf.first);
            attr.samples = samples;
            buf.second = renderAllocator.createTextureBuffer(attr);
        }

        //Reattach textures
        if (!currentDepthStencil.empty())
            renderTarget->attachDepthStencil(*buffers.at(currentDepthStencil));

        renderTarget->setNumberOfColorAttachments(static_cast<int>(currentColor.size()));
        for (int i = 0; i < currentColor.size(); i++) {
            renderTarget->attachColor(i, *buffers.at(currentColor.at(i)));
        }
    }
}