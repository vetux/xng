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

#ifndef XENGINE_RAWRESOURCE_HPP
#define XENGINE_RAWRESOURCE_HPP

#include "xng/resource/resource.hpp"

namespace xng {
    /**
     * A raw resource makes the byte data of the resource available for custom parsing.
     */
    struct RawResource : public Resource {
        std::vector<uint8_t> bytes;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<RawResource>(*this);
        }

        std::type_index getTypeIndex() override {
            return typeid(RawResource);
        }
    };
}

#endif //XENGINE_RAWRESOURCE_HPP
