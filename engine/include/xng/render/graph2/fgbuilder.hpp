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

#ifndef XENGINE_FGBUILDER_HPP
#define XENGINE_FGBUILDER_HPP

#include <string>
#include <functional>

#include "xng/render/graph2/fggraph.hpp"
#include "xng/render/graph2/fgcontext.hpp"
#include "xng/render/graph2/fgresource.hpp"

#include "xng/render/graph2/shader/fgshadersource.hpp"
#include "xng/render/graph2/texture/fgtexture.hpp"

namespace xng {
    class XENGINE_EXPORT FGBuilder {
    public:
        typedef size_t PassHandle;

        FGBuilder();

        ~FGBuilder() = default;

        /**
         * Declare a previously allocated resource to be inherited by this graph.
         *
         * The specified resource must have been allocated or inherited in the
         * preceding FGRuntime.compile() or FGRuntime.recompile() call.
         *
         * @param resource The resource to inherit
         * @return The new resource handle representing the inherited resource
         */
        FGResource inheritResource(FGResource resource);

        FGResource createVertexBuffer(size_t size);

        FGResource createIndexBuffer(size_t size);

        FGResource createShaderBuffer(size_t size);

        FGResource createTexture(const FGTexture &texture);

        FGResource createShader(const FGShaderSource &shader);

        /**
         * Create a render pipeline from the given shader resources.
         *
         * All shaders in a pipeline are required to have identical buffer / texture / parameter layouts and compatible
         * input / output attribute layouts.
         *
         * @param shaders
         * @return
         */
        FGResource createPipeline(const std::unordered_set<FGResource> &shaders);

        /**
         * The screen texture is an offscreen texture managed by the runtime.
         * It can be read from and written to by render passes to access the resulting screen contents.
         *
         * The texture format is as follows:
         *  .size = Window / Full-screen size (Determined by the display environment)
         *  .textureType = TEXTURE2D
         *  .format = RGBA
         *
         * @return The offscreen texture representing the screen.
         */
        FGResource getScreenTexture() const;

        PassHandle addPass(const std::string &name, std::function<void(FGContext &)> pass);

        void read(PassHandle pass, FGResource resource);

        void write(PassHandle pass, FGResource resource);

        void readWrite(PassHandle pass, FGResource resource);

        FGGraph build();

    private:
        FGResource createResource();

        FGResource resourceCounter;

        std::vector<FGPass> passes;

        std::unordered_map<FGResource, size_t> vertexBufferAllocation;
        std::unordered_map<FGResource, size_t> indexBufferAllocation;
        std::unordered_map<FGResource, size_t> shaderBufferAllocation;

        std::unordered_map<FGResource, FGTexture> textureAllocation;

        std::unordered_map<FGResource, FGShaderSource> shaderAllocation;
        std::unordered_map<FGResource, std::unordered_set<FGResource> > pipelineAllocation;

        std::unordered_map<FGResource, FGResource> inheritedResources;

        FGResource screenTexture{};
    };
}

#endif //XENGINE_FGBUILDER_HPP
