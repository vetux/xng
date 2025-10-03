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

#ifndef XENGINE_CRYPTOPPGZIP_HPP
#define XENGINE_CRYPTOPPGZIP_HPP

#include "xng/crypto/gzip.hpp"

#include "cryptopp/filters.h"
#include "cryptopp/gzip.h"
#include "cryptopp/cryptlib.h"

namespace xng {
    class CryptoPPGzip : public GZip {
    public:
        std::vector<char> compress(const char *data, size_t length)override {
            std::string compressed;
            CryptoPP::Gzip zipper(new CryptoPP::StringSink(compressed));
            zipper.Put((CryptoPP::byte *) data, length);
            zipper.MessageEnd();
            return {compressed.begin(), compressed.end()};
        }

        std::vector<char> decompress(const char *data, size_t length) override{
            std::string decompressed;
            CryptoPP::Gunzip unzip(new CryptoPP::StringSink(decompressed));
            unzip.Put((CryptoPP::byte *) data, length);
            unzip.MessageEnd();
            return {decompressed.begin(), decompressed.end()};
        }

        std::string compress(const std::string &data)override {
            auto r = compress(data.data(), data.size());
            return {r.begin(), r.end()};
        }

        std::string decompress(const std::string &data)override {
            auto r = decompress(data.data(), data.size());
            return {r.begin(), r.end()};
        }

        std::vector<char> compress(const std::vector<char> &data) override{
            return compress(data.data(), data.size());
        }

        std::vector<char> decompress(const std::vector<char> &data)override {
            return decompress(data.data(), data.size());
        }
    };
}

#endif //XENGINE_CRYPTOPPGZIP_HPP
