/**
 *  xEngine - C++ game engine library
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

#ifndef XENGINE_CRC_HPP
#define XENGINE_CRC_HPP

//https://rosettacode.org/wiki/CRC-32#C.2B.2B

#include <algorithm>
#include <array>
#include <cstdint>
#include <numeric>

namespace xengine {
    typedef std::uint_fast32_t HashCRC;

    // Generates a lookup table for the checksums of all 8-bit values.
    std::array<HashCRC, 256> generate_crc_lookup_table() noexcept {
        auto const reversed_polynomial = HashCRC{0xEDB88320uL};

        // This is a function object that calculates the checksum for a value,
        // then increments the value, starting from zero.
        struct byte_checksum {
            HashCRC operator()() noexcept {
                auto checksum = static_cast<HashCRC>(n++);

                for (auto i = 0; i < 8; ++i)
                    checksum = (checksum >> 1) ^ ((checksum & 0x1u) ? reversed_polynomial : 0);

                return checksum;
            }

            unsigned n = 0;
        };

        auto table = std::array<HashCRC, 256>{};
        std::generate(table.begin(), table.end(), byte_checksum{});

        return table;
    }

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
