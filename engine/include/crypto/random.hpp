/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_RANDOM_HPP
#define XENGINE_RANDOM_HPP

#include <cstddef>
#include <memory>
#include <vector>
#include <limits>

namespace xng {
    /**
     * Cryptographically secure random number generator.
     */
    class XENGINE_EXPORT Random {
    public:
        virtual ~Random() = default;

        virtual unsigned char byte() = 0;

        virtual unsigned int bit() = 0;

        virtual unsigned int word(unsigned int min = 0,
                                  unsigned int max = std::numeric_limits<unsigned int>::max()) = 0;

        virtual void block(unsigned char *data, size_t size) = 0;

        virtual void discard(size_t size) = 0;
    };
}

#endif //XENGINE_RANDOM_HPP
