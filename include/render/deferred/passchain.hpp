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

#ifndef XENGINE_PASSCHAIN_HPP
#define XENGINE_PASSCHAIN_HPP

#include <memory>
#include <vector>

#include "platform/graphics/texturebuffer.hpp"

#include "render/deferred/renderpass.hpp"

namespace xengine {
    struct XENGINE_EXPORT PassChain {
        struct Node {
            std::shared_ptr<TextureBuffer> color;
            bool enableBlending = true;
            BlendMode colorBlendModeSource = BlendMode::SRC_ALPHA;
            BlendMode colorBlendModeDest = BlendMode::ONE_MINUS_SRC_ALPHA;

            std::shared_ptr<TextureBuffer> depth;
            DepthTestMode depthTestMode = DepthTestMode::DEPTH_TEST_LESS;
        };

        /**
         * Return the nodes vector for the stored passes in order.
         *
         * Users may override this function to access custom pass members.
         *
         * @return
         */
        virtual std::vector<Node> getNodes() {
            std::vector<Node> ret;
            for (auto &pass: passes) {
                Node n;
                n.color = pass->getColorBuffer();
                n.depth = pass->getDepthBuffer();
                ret.emplace_back(n);
            }
            return ret;
        }

        std::vector<std::unique_ptr<RenderPass>> passes; // The passes in order
    };
}

#endif //XENGINE_PASSCHAIN_HPP
