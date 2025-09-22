/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_ENTITYHANDLE_HPP
#define XENGINE_ENTITYHANDLE_HPP

#include <string>

#include "xng/util/hashcombine.hpp"

namespace xng {
    struct XENGINE_EXPORT EntityHandle {
        int id;

        EntityHandle() : id(-1) {}

        explicit EntityHandle(int id) : id(id) {}

        bool operator<(const EntityHandle &other) const {
            return id < other.id;
        }

        bool operator==(const EntityHandle &other) const {
            return id == other.id;
        }

        bool operator!=(const EntityHandle &other) const {
            return id != other.id;
        }

        explicit operator bool() const {
            return id != -1;
        }

        std::string toString() const {
            return std::to_string(id);
        }
    };

    class EntityHandleHash {
    public:
        std::size_t operator()(const EntityHandle &k) const {
            size_t ret = 0;
            hash_combine(ret, k.id);
            return ret;
        }
    };
}

#endif //XENGINE_ENTITYHANDLE_HPP
