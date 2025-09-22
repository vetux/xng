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

#ifndef XENGINE_CRYPTOPROVIDER_HPP
#define XENGINE_CRYPTOPROVIDER_HPP

#include "xng/crypto/aes.hpp"
#include "xng/crypto/gzip.hpp"
#include "xng/crypto/random.hpp"
#include "xng/crypto/sha.hpp"

namespace xng {
    class CryptoProvider {
    public:
        virtual std::unique_ptr<AES> createAES() = 0;

        virtual std::unique_ptr<GZip> createGzip() = 0;

        /**
         * Create a random generator instance and seed it using os supplied entropy.
         * @return
         */
        virtual std::unique_ptr<Random> createRandom() = 0;

        virtual std::unique_ptr<SHA> createSHA() = 0;
    };
}

#endif //XENGINE_CRYPTOPROVIDER_HPP
