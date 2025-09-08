/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "contextgl.hpp"

void ContextGL::uploadBuffer(FGResource buffer, const uint8_t *ptr, size_t size) {
}

void ContextGL::uploadTexture(FGResource texture, const uint8_t *ptr, size_t size, FGColorFormat format, size_t index,
                              size_t mipMapLevel, FGCubeMapFace face) {
}

void ContextGL::bindVertexBuffer(FGResource buffer) {
}

void ContextGL::bindIndexBuffer(FGResource buffer) {
}

void ContextGL::bindRenderTarget(size_t binding,
                                 FGResource texture,
                                 size_t index,
                                 size_t mipMapLevel,
                                 FGCubeMapFace face) {
}

void ContextGL::bindTextures(const std::unordered_map<std::string, FGResource> &textures) {
}

void ContextGL::bindShaderBuffers(const std::unordered_map<std::string, FGResource> &buffers) {
}

void ContextGL::setShaderParameters(const std::unordered_map<std::string, FGShaderLiteral> &parameters) {
}

void ContextGL::bindShaders(const std::vector<FGResource> &shaders) {
}

void ContextGL::draw(const std::vector<FGDrawCall> &calls) {
}

std::vector<uint8_t> ContextGL::downloadShaderBuffer(FGResource buffer) {
}

Image<ColorRGBA> ContextGL::downloadTexture(FGResource texture, size_t index, size_t mipMapLevel, FGCubeMapFace face) {
}

std::string ContextGL::getShaderSource(FGResource shader) {
    return shaders.at(shader);
}
