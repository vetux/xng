/**
 *  XEngine - C++ game engine library
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

#ifndef XENGINE_ENTITY_HPP
#define XENGINE_ENTITY_HPP

namespace xengine {
    struct XENGINE_EXPORT Entity {
        static const int INVALID_ID = -1;

        int id;

        Entity() : id(INVALID_ID) {}

        explicit Entity(int id) : id(id) {}

        bool operator<(const Entity &other) const {
            return id < other.id;
        }
    };
}
#endif //XENGINE_ENTITY_HPP
