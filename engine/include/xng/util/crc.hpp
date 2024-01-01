/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_CRC_HPP
#define XENGINE_CRC_HPP

//https://rosettacode.org/wiki/CRC-32#C.2B.2B

#include <algorithm>
#include <array>
#include <cstdint>
#include <numeric>

namespace xng {
    typedef std::uint_fast32_t HashCRC;

    // Generates a lookup table for the checksums of all 8-bit values.
    std::array<HashCRC, 256> generate_crc_lookup_table() noexcept;

// Calculates the CRC for any sequence of values. (You could use type traits and a
// static assert to ensure the values can be converted to 8 bits.)
    template<typename InputIterator>
    HashCRC crc(InputIterator first, InputIterator last) {
        // Generate lookup table only on first use then cache it - this is thread-safe.
        static auto const table = generate_crc_lookup_table();

        // Calculate the checksum - make sure to clip to 32 bits, for systems that don't
        // have a true (fast) 32-bit type.
        return HashCRC{0xFFFFFFFFuL} &
               ~std::accumulate(first, last,
                                ~HashCRC{0} & HashCRC{0xFFFFFFFFuL},
                                [](HashCRC checksum, std::uint_fast8_t value) {
                                    return table[(checksum ^ value) & 0xFFu] ^ (checksum >> 8);
                                });
    }
}

#endif //XENGINE_CRC_HPP
