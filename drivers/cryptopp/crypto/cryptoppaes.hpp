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

#ifndef XENGINE_CRYPTOPPAES_HPP
#define XENGINE_CRYPTOPPAES_HPP

#include "xng/crypto/aes.hpp"

#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include "cryptopp/modes.h"
#include "cryptopp/cryptlib.h"

namespace xng {
    static std::vector<CryptoPP::byte> parseKey(const std::string &inKey) {
        auto keyLength = CryptoPP::AES::StaticGetValidKeyLength(inKey.size());
        std::vector<CryptoPP::byte> key(keyLength, 0);
        for (auto i = 0; i < inKey.size() && i < key.size(); i++) {
            key.at(i) = inKey.at(i);
        }
        return key;
    }

    static std::array<CryptoPP::byte, CryptoPP::AES::BLOCKSIZE>
    parseIv(const std::array<unsigned char, AES::BLOCKSIZE> &inIv) {
        std::array<CryptoPP::byte, CryptoPP::AES::BLOCKSIZE> iv{};
        for (auto i = 0; i < iv.size(); i++) {
            iv.at(i) = inIv.at(i);
        }
        return iv;
    }

    class CryptoPPAES : public AES {
        std::string encrypt(const std::string &inKey,
                            const InitializationVector &inIv,
                            const std::string &plaintext) override {
            std::string ciphertext;

            auto key = parseKey(inKey);
            auto iv = parseIv(inIv);

            CryptoPP::AES::Encryption aesEncryption(key.data(), key.size());
            CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv.data());

            CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
            stfEncryptor.Put(reinterpret_cast<const unsigned char *>(plaintext.c_str()), plaintext.length());
            stfEncryptor.MessageEnd();

            return ciphertext;
        }

        std::string decrypt(const std::string &inKey,
                            const InitializationVector &inIv,
                            const std::string &ciphertext) override {
            std::string plaintext;

            auto key = parseKey(inKey);
            auto iv = parseIv(inIv);

            CryptoPP::AES::Decryption aesDecryption(key.data(), key.size());
            CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv.data());

            CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(plaintext));
            stfDecryptor.Put(reinterpret_cast<const unsigned char *>(ciphertext.c_str()), ciphertext.size());
            stfDecryptor.MessageEnd();

            return plaintext;
        }

        std::vector<char> encrypt(const std::string &key,
                                  const InitializationVector &iv,
                                  const std::vector<char> &plaintext) override {
            auto ret = encrypt(key, iv, std::string(plaintext.begin(), plaintext.end()));
            return {ret.begin(), ret.end()};
        }

        std::vector<char> decrypt(const std::string &key,
                                  const InitializationVector &iv,
                                  const std::vector<char> &ciphertext) override {
            auto ret = decrypt(key, iv, std::string(ciphertext.begin(), ciphertext.end()));
            return {ret.begin(), ret.end()};
        }
    };
}

#endif //XENGINE_CRYPTOPPAES_HPP
