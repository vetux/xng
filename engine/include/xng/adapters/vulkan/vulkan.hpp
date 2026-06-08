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

#ifndef XENGINE_VULKAN_HPP
#define XENGINE_VULKAN_HPP

#include "xng/rendergraph/runtime.hpp"

namespace xng::vulkan {
    class XENGINE_EXPORT Runtime final : public xng::rg::Runtime {
    public:
        explicit Runtime(const std::vector<std::string> &extensions);

        ~Runtime() override;

        std::shared_ptr<rg::Surface> createSurface(std::shared_ptr<Window> window, size_t swapCount) override;

        rg::Heap &getResourceHeap() override;

        rg::PipelineCache &getPipelineCache() override;

        std::unordered_set<rg::ColorFormat> getSupportedColorFormats() override;

        rg::TextureFormatLimits getTextureFormatLimits(rg::TextureType type,
                                                       rg::ColorFormat colorFormat,
                                                       rg::Texture::Capability capabilities) override;

        std::unique_ptr<rg::Semaphore> execute(const rg::Graph &graph) override;

        std::unique_ptr<rg::Semaphore> execute(const std::vector<rg::Graph> &graphs) override;
    };
}

#endif //XENGINE_VULKAN_HPP
