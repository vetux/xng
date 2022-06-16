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

#ifndef XENGINE_RENDERTARGETDESC_HPP
#define XENGINE_RENDERTARGETDESC_HPP

#include "math/vector2.hpp"

#include "algo/hashcombine.hpp"

namespace xengine {
    struct RenderTargetDesc {
        Vec2i size{};
        bool multisample = false;
        int samples = 0;
        int numberOfColorAttachments = 1;
        bool hasDepthStencilAttachment = false;

        bool operator==(const RenderTargetDesc &other) const {
            return size == other.size
                   && multisample == other.multisample
                   && samples == other.samples
                   && numberOfColorAttachments == other.numberOfColorAttachments
                   && hasDepthStencilAttachment == other.hasDepthStencilAttachment;
        }
    };
}

using namespace xengine;
namespace std {
    template<>
    struct hash<RenderTargetDesc> {
        std::size_t operator()(const RenderTargetDesc &k) const {
            size_t ret = 0;
            hash_combine(ret, k.size.x);
            hash_combine(ret, k.size.y);
            hash_combine(ret, k.multisample);
            hash_combine(ret, k.samples);
            hash_combine(ret, k.numberOfColorAttachments);
            hash_combine(ret, k.hasDepthStencilAttachment);
            return ret;
        }
    };
}

#endif //XENGINE_RENDERTARGETDESC_HPP
