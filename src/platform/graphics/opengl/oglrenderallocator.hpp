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

#ifndef XENGINE_OGLRENDERALLOCATOR_HPP
#define XENGINE_OGLRENDERALLOCATOR_HPP

#include "platform/graphics/renderallocator.hpp"

#include "asset/mesh.hpp"

namespace xengine {
    namespace opengl {
        class OGLRenderAllocator : public RenderAllocator {
        public:
            std::unique_ptr<RenderTarget> createRenderTarget(Vec2i size) override;

            std::unique_ptr<RenderTarget> createRenderTarget(Vec2i size, int samples) override;

            std::unique_ptr<TextureBuffer> createTextureBuffer(TextureBuffer::Attributes attributes) override;

            std::unique_ptr<MeshBuffer> createMeshBuffer(const Mesh &mesh) override;

            std::unique_ptr<MeshBuffer> createInstancedMeshBuffer(const Mesh &mesh,
                                                                  const std::vector<Transform> &offsets) override;

            std::unique_ptr<MeshBuffer> createCustomMeshBuffer(const CustomMeshDefinition &mesh) override;

            std::unique_ptr<ShaderProgram> createShaderProgram(const ShaderSource &vertexShader,
                                                               const ShaderSource &fragmentShader) override;

            std::unique_ptr<ShaderProgram> createShaderProgram(const ShaderSource &vertexShader,
                                                               const ShaderSource &geometryShader,
                                                               const ShaderSource &fragmentShader) override;

            std::unique_ptr<ShaderProgram> createShaderProgram(const ShaderBinary &shader) override;
        };
    }
}
#endif //XENGINE_OGLRENDERALLOCATOR_HPP
