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

#ifndef XENGINE_FONTDATA_HPP
#define XENGINE_FONTDATA_HPP

#include <utility>
#include <vector>

#include "xng/resource/resourcebase.hpp"

namespace xng {
    struct Font final : ResourceBase {
        RESOURCE_TYPENAME(Font)

        std::vector<uint8_t> data;

        Font() = default;

        explicit Font(std::vector<uint8_t> data) : data(std::move(data)) {}

        std::unique_ptr<ResourceBase> clone() override {
            return std::make_unique<Font>(*this);
        }
    };
}

#endif //XENGINE_FONTDATA_HPP
