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

#ifndef XENGINE_OBJECTSTD140_HPP
#define XENGINE_OBJECTSTD140_HPP

#include <cassert>
#include <cstring>
#include <vector>

#include "xng/shaderscript/layoutstd140.hpp"

namespace xng {
    class ObjectStd140 {
    public:
        explicit ObjectStd140(const LayoutStd140 &layout)
            : layout(layout), data(layout.getTotalSize()) {
        }

        template<typename T>
        void set(const std::string &name, const T &value) {
            assert(std::holds_alternative<rg::ShaderPrimitiveType>(layout.getStructType().get(name).type.value));
            if (layout.getStructType().get(name).type.getPrimitive() != rg::ShaderPrimitive(value).getType()) {
                throw std::runtime_error("Invalid member type for " + name);
            }

            Std140<T> valueAligned(value);

            const auto offset = layout.getOffset(name);

            assert(offset + sizeof(Std140<T>) <= data.size());

            std::memcpy(data.data() + offset, &valueAligned, sizeof(Std140<T>));
        }

    private:
        LayoutStd140 layout;
        std::vector<uint8_t> data;
    };
}

#endif //XENGINE_OBJECTSTD140_HPP
