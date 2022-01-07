/**
 *  Mana - 3D Game Engine
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

#include "compression/gzip.hpp"

#include "cryptopp/filters.h"
#include "cryptopp/gzip.h"
#include "cryptopp/cryptlib.h"

namespace engine {
    std::vector<char> GZip::compress(const char *data, size_t length) {
        std::string compressed;
        CryptoPP::Gzip zipper(new CryptoPP::StringSink(compressed));
        zipper.Put((CryptoPP::byte *) data, length);
        zipper.MessageEnd();
        return {compressed.begin(), compressed.end()};
    }

    std::vector<char> GZip::decompress(const char *data, size_t length) {
        std::string decompressed;
        CryptoPP::Gunzip unzip(new CryptoPP::StringSink(decompressed));
        unzip.Put((CryptoPP::byte *) data, length);
        unzip.MessageEnd();
        return {decompressed.begin(), decompressed.end()};
    }

    std::string GZip::compress(const std::string &data) {
        auto r = compress(data.data(), data.size());
        return {r.begin(), r.end()};
    }

    std::string GZip::decompress(const std::string &data) {
        auto r = decompress(data.data(), data.size());
        return {r.begin(), r.end()};
    }

    std::vector<char> GZip::compress(const std::vector<char> &data) {
        return compress(data.data(), data.size());
    }

    std::vector<char> GZip::decompress(const std::vector<char> &data) {
        return decompress(data.data(), data.size());
    }
}