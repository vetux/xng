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

#include "xng/util/crc.hpp"

namespace xng {
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
}

