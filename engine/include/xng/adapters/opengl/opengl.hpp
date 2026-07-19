/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_OPENGL_HPP
#define XENGINE_OPENGL_HPP

#include "xng/rendergraph/runtime.hpp"

#include "xng/display/displayenvironment.hpp"

namespace xng::opengl {
    class XENGINE_EXPORT Runtime final : public rg::Runtime {
    public:
        /**
         * To be able to implement the heap, the runtime must create a hidden window
         * because of tight context/window coupling in gl.
         *
         * @param env The env used to create the heap hidden window.
         */
        explicit Runtime(DisplayEnvironment &env);

        ~Runtime() override;

        std::shared_ptr<rg::Surface> createSurface(std::shared_ptr<Window> window, size_t swapCount) override;

        void setEnableTimers(bool enableTimers) override;

        rg::Heap &getResourceHeap() override;

        rg::PipelineCache &getPipelineCache() override;

        std::unordered_set<rg::ColorFormat> getSupportedColorFormats() override;

        rg::TextureFormatLimits getTextureFormatLimits(rg::TextureType type,
                                                       rg::ColorFormat colorFormat,
                                                       rg::Texture::Capability capabilities) override;

        std::unique_ptr<rg::Fence> execute(const rg::Graph &graph) override;

        std::unique_ptr<rg::Fence> execute(const std::vector<rg::Graph> &graphs) override;

    private:
        struct MemberData;
        std::unique_ptr<MemberData> data;
    };
}

#endif //XENGINE_OPENGL_HPP
