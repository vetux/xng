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

#ifndef XENGINE_RENDER2DPASS_HPP
#define XENGINE_RENDER2DPASS_HPP

#include "xng/render/renderpass.hpp"
#include "xng/render/2d/renderbatch2d.hpp"
#include "xng/render/2d/meshbuffer2d.hpp"
#include "xng/render/atlas/textureatlas.hpp"
#include "xng/render/geometry/primitive.hpp"

namespace xng {
    class XENGINE_EXPORT RenderPass2D final : public RenderPass {
    public:
        RenderPass2D();

        /**
         * Has to be called immediately before executing the render graph runtime.
         *
         * @return True if the graph must be rebuilt.
         */
        bool shouldRebuild() override;

        void create(RenderGraphBuilder &builder) override;

        void recreate(RenderGraphBuilder &builder) override;

        /**
         * Set the batches to be rendered.
         *
         * Should be called before calling shouldRebuild()
         *
         * @param renderBatches The batches to be rendered.
         */
        void setBatches(const std::vector<RenderBatch2D> &renderBatches);

    private:
        struct MeshDrawData {
            Primitive primitive{};
            DrawCall drawCall{};
            size_t baseVertex{};

            MeshDrawData() = default;

            MeshDrawData(const Primitive primitive, const DrawCall &drawCall,
                         const size_t baseVertex) : primitive(primitive),
                                                    drawCall(drawCall),
                                                    baseVertex(baseVertex) {
            }
        };

        class RotationPairHash {
        public:
            std::size_t operator()(const std::pair<float, Vec2f> &k) const {
                size_t ret = 0;
                hash_combine(ret, k.first);
                hash_combine(ret, k.second);
                return ret;
            }
        };

        Mat4f getRotationMatrix(float rotation, const Vec2f& center);

        void runPass(RenderGraphContext &ctx);

        RenderGraphResource screenTexture;

        RenderGraphResource trianglePipeline{};
        RenderGraphResource linePipeline{};
        RenderGraphResource pointPipeline{};

        RenderGraphResource vertexBuffer{};
        RenderGraphResource indexBuffer{};

        RenderGraphResource vertexBufferCopy{};
        RenderGraphResource indexBufferCopy{};

        size_t vertexBufferSize{};
        size_t indexBufferSize{};

        RenderGraphResource shaderBuffer{};

        std::unordered_map<std::pair<float, Vec2f>, Mat4f, RotationPairHash> rotationMatrices;
        std::unordered_set<std::pair<float, Vec2f>, RotationPairHash> usedRotationMatrices;

        MeshBuffer2D meshBuffer;

        TextureAtlas atlas;
        std::vector<RenderBatch2D> batches;

        std::unordered_map<Texture2D::Handle, TextureAtlasHandle> atlasHandles;
    };
}

#endif //XENGINE_RENDER2DPASS_HPP
