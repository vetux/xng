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

#ifndef MANA_AES_HPP
#define MANA_AES_HPP

#include <string>
#include <vector>
#include <array>

namespace engine {
    namespace AES {
        const int BLOCKSIZE = 128;

        typedef std::string Key;
        typedef std::array<char, BLOCKSIZE> InitializationVector;

        MANA_EXPORT std::string encrypt(const Key &key, const InitializationVector &iv, const std::string &plaintext);

        MANA_EXPORT std::string decrypt(const Key &key, const InitializationVector &iv, const std::string &ciphertext);

        MANA_EXPORT std::vector<char> encrypt(const Key &key, const InitializationVector &iv,
                                              const std::vector<char> &plaintext);

        MANA_EXPORT std::vector<char> decrypt(const Key &key, const InitializationVector &iv,
                                              const std::vector<char> &ciphertext);
    }
}

#endif //MANA_AES_HPP
