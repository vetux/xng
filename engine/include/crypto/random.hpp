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
        // To avoid forward declaration of cryptopp types use polymorphic member type
        struct Member {
            virtual ~Member() = default;
        };

        /**
         * Create the generator instance and seed it using os supplied entropy.
         */
        Random();

        unsigned char byte();

        unsigned int bit();

        unsigned int word(unsigned int min = 0, unsigned int max = std::numeric_limits<unsigned int>::max());

        void block(unsigned char *data, size_t size);

        void discard(size_t size);

    private:
        template<typename T>
        T &getMembers() {
            return dynamic_cast<T &>(*members);
        }

        std::unique_ptr<Member> members;
    };
}

#endif //XENGINE_RANDOM_HPP
