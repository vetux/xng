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

#ifndef XENGINE_FRAMEGRAPHLAYER_HPP
#define XENGINE_FRAMEGRAPHLAYER_HPP

#include "render/platform/texturebuffer.hpp"

namespace xengine {
    struct XENGINE_EXPORT FrameGraphLayer {
        FrameGraphResource color;
        FrameGraphResource depth;
        bool enableBlending = true;
        BlendMode colorBlendModeSource = BlendMode::SRC_ALPHA;
        BlendMode colorBlendModeDest = BlendMode::ONE_MINUS_SRC_ALPHA;
        DepthTestMode depthTestMode = DepthTestMode::DEPTH_TEST_LESS;
    };
}
#endif //XENGINE_FRAMEGRAPHLAYER_HPP
