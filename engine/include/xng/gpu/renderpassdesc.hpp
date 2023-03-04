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

#ifndef XENGINE_RENDERPASSDESC_HPP
#define XENGINE_RENDERPASSDESC_HPP

namespace xng {
    struct RenderPassDesc {
        int numberOfColorAttachments = 1;
        bool hasDepthStencilAttachment = false;

        bool operator==(const RenderTargetDesc &other) const {
            return numberOfColorAttachments == other.numberOfColorAttachments
                   && hasDepthStencilAttachment == other.hasDepthStencilAttachment;
        }
    };
}

namespace std {
    template<>
    struct hash<xng::RenderPassDesc> {
        std::size_t operator()(const xng::RenderPassDesc &k) const {
            size_t ret = 0;

            xng::hash_combine(ret, k.numberOfColorAttachments);
            xng::hash_combine(ret, k.hasDepthStencilAttachment);

            return ret;
        }
    };
}

#endif //XENGINE_RENDERPASSDESC_HPP
