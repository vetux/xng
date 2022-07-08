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

#include "crypto/random.hpp"

#include <cryptopp/osrng.h>

namespace xng {
    struct MemberCryptoPP : public Random::Member {
        CryptoPP::AutoSeededRandomPool pool;
    };

    Random::Random()
            : members(std::make_unique<MemberCryptoPP>()) {}

    unsigned char Random::byte() {
        return getMembers<MemberCryptoPP>().pool.GenerateByte();
    }

    unsigned int Random::bit() {
        return getMembers<MemberCryptoPP>().pool.GenerateBit();
    }

    unsigned int Random::word(unsigned int min, unsigned int max) {
        return getMembers<MemberCryptoPP>().pool.GenerateWord32(min, max);
    }

    void Random::block(unsigned char *data, size_t size) {
        getMembers<MemberCryptoPP>().pool.GenerateBlock(data, size);
    }

    void Random::discard(size_t size) {
        getMembers<MemberCryptoPP>().pool.DiscardBytes(size);
    }
}