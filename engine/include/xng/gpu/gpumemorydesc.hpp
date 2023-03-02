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

#ifndef XENGINE_GPUMEMORYDESC_HPP
#define XENGINE_GPUMEMORYDESC_HPP

namespace xng {
    struct GpuMemoryDesc {
        size_t size; // The size of this memory object
        std::set<RenderObject::Type> bufferTypes; // The list of buffer types which can be created in this memory object

        bool operator==(const GpuMemoryDesc &other) const {
            return size == other.size
                   && bufferTypes == other.bufferTypes;
        }
    };
}

namespace std {
    template<>
    struct hash<xng::GpuMemoryDesc> {
        std::size_t operator()(const xng::GpuMemoryDesc &k) const {
            size_t ret = 0;
            xng::hash_combine(ret, k.size);
            for (auto &v : k.bufferTypes)
                xng::hash_combine(ret, v);
            return ret;
        }
    };
}

#endif //XENGINE_GPUMEMORYDESC_HPP
