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

#ifndef XENGINE_FRAMEGRAPHPOOL_HPP
#define XENGINE_FRAMEGRAPHPOOL_HPP

#include <unordered_set>

#include "resource/resourcehandle.hpp"

#include "gpu/renderdevice.hpp"

#include "asset/texture.hpp"
#include "asset/mesh.hpp"
#include "asset/shader.hpp"

namespace xng {
    class XENGINE_EXPORT FrameGraphPool {
    public:
        FrameGraphPool() = default;

        FrameGraphPool(RenderDevice &device, SPIRVCompiler &shaderCompiler, SPIRVDecompiler &spirvDecompiler);

        FrameGraphPool(const FrameGraphPool &other) = delete;

        FrameGraphPool &operator=(const FrameGraphPool &other) = delete;

        /**
         * Deallocates buffers for resource handles which have not been used since the last call to collectGarbage.
         */
        void collectGarbage();

        /// ResourceHandle buffers are allocated with DEVICE_LOCAL storage.

        VertexBuffer &getMesh(const ResourceHandle<Mesh> &handle);

        TextureBuffer &getTexture(const ResourceHandle<Texture> &handle);

        /**
         * @param shader
         * @param descIn
         * @return The RenderPipeline instance for the given description
         */
        RenderPipeline &getPipeline(const ResourceHandle<Shader> &shader, const RenderPipelineDesc &desc);

        /**
         * Each call creates a new instance of TextureBuffer.
         *
         * @param desc
         * @return A new TextureBuffer instance
         */
        TextureBuffer &createTextureBuffer(const TextureBufferDesc &desc);

        /**
         * @param desc
         * @return A new ShaderBuffer instance
         */
        ShaderBuffer &createShaderBuffer(const ShaderBufferDesc &desc);

        /**
         * Each call creates a new instance of RenderTarget.
         *
         * @param desc
         * @return A new RenderTarget instance
         */
        RenderTarget &createRenderTarget(const RenderTargetDesc &desc);

        void destroy(RenderObject &obj);

    private:
        ShaderProgram &getShader(const ResourceHandle<Shader> &handle);

        struct PipelinePair {
            Uri uri;
            RenderPipelineDesc desc;

            PipelinePair() = default;

            PipelinePair(Uri uri, RenderPipelineDesc desc) : uri(std::move(uri)), desc(std::move(desc)) {}

            ~PipelinePair() = default;

            bool operator==(const PipelinePair &other) const {
                return uri == other.uri && desc == other.desc;
            }
        };

        class PipelinePairHash {
        public:
            std::size_t operator()(const PipelinePair &k) const {
                size_t ret = 0;
                hash_combine(ret, k.uri);
                hash_combine(ret, k.desc);
                return ret;
            }
        };

        RenderDevice *device = nullptr;
        SPIRVCompiler *shaderCompiler = nullptr;
        SPIRVDecompiler *shaderDecompiler = nullptr;

        std::unordered_map<Uri, std::unique_ptr<RenderObject>> uriObjects;
        std::unordered_map<PipelinePair, std::unique_ptr<RenderPipeline>, PipelinePairHash> pipelines;

        std::unordered_map<TextureBufferDesc, std::vector<std::unique_ptr<TextureBuffer>>> textures;
        std::unordered_map<ShaderBufferDesc, std::vector<std::unique_ptr<ShaderBuffer>>> shaderBuffers;
        std::unordered_map<RenderTargetDesc, std::vector<std::unique_ptr<RenderTarget>>> targets;

        std::set<Uri> usedUris;
        std::unordered_set<PipelinePair, PipelinePairHash> usedPipelines;

        std::unordered_map<TextureBufferDesc, int> usedTextures;
        std::unordered_map<ShaderBufferDesc, int> usedShaderBuffers;
        std::unordered_map<RenderTargetDesc, int> usedTargets;
    };
}
#endif //XENGINE_FRAMEGRAPHPOOL_HPP
