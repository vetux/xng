/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_AES_HPP
#define XENGINE_AES_HPP

#include <string>
#include <vector>
#include <array>

#include "crypto/random.hpp"

namespace xng {
    /**
     * AES Encryption and Decryption
     */
    class XENGINE_EXPORT AES {
    public:
        static const int BLOCKSIZE = 128;

        typedef std::string Key;
        typedef std::array<unsigned char, BLOCKSIZE> InitializationVector;

        virtual ~AES() = default;

        virtual std::string encrypt(const Key &key,
                                    const InitializationVector &iv,
                                    const std::string &plaintext) = 0;

        virtual std::string decrypt(const Key &key,
                                    const InitializationVector &iv,
                                    const std::string &ciphertext) = 0;

        virtual std::vector<char> encrypt(const Key &key,
                                          const InitializationVector &iv,
                                          const std::vector<char> &plaintext) = 0;

        virtual std::vector<char> decrypt(const Key &key,
                                          const InitializationVector &iv,
                                          const std::vector<char> &ciphertext) = 0;

        /**
         * @param rand The random generator to use when generating the iv data
         * @return Cryptographically secure random iv
         */
        static InitializationVector getRandomIv(Random &rand) {
            InitializationVector ret;
            rand.block(ret.data(), ret.size());
            return ret;
        }
    };
}

#endif //XENGINE_AES_HPP
