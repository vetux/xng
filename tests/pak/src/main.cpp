/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/xng.hpp"

#include <fstream>

int main(int argc, char *argv[]) {
    auto cryptoDriver = xng::cryptopp::CryptoPPDriver();
    auto sha = cryptoDriver.createSHA();
    auto aes = cryptoDriver.createAES();
    auto zip = cryptoDriver.createGzip();
    auto ran = cryptoDriver.createRandom();

    std::set<std::string> paths;
    xng::PakBuilder builder;
    for (auto &path: std::filesystem::recursive_directory_iterator("assets/")) {
        if (path.is_directory())
            continue;
        auto p = path.path().string();
        paths.insert(p);
        builder.addEntry(p, xng::readFile(p));
    }

    auto pakData = builder.build(0,
                                 true,
                                 false,
                                 *sha,
                                 *zip,
                                 *aes,
                                 "test",
                                 xng::AES::getRandomIv(*ran));

    auto pakDataE = builder.build(0,
                                  true,
                                  true,
                                  *sha,
                                  *zip,
                                  *aes,
                                  "test",
                                  xng::AES::getRandomIv(*ran));

    std::ofstream ofstream("assets.pak");
    ofstream.write(pakData.at(0).data(), static_cast<std::streamsize>(pakData.at(0).size()));
    ofstream.close();

    ofstream = std::ofstream("assets_encrypted.pak");
    ofstream.write(pakDataE.at(0).data(), static_cast<std::streamsize>(pakDataE.at(0).size()));
    ofstream.close();

    std::ifstream stream("assets.pak", std::ios_base::in | std::ios::binary);
    xng::Pak pak(stream, *zip, *sha, *aes, "test");
    for (auto &pair: pak.getEntries()) {
        auto data = pak.get(pair.first, true);
        auto path = "extract/" + pair.first;
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        std::ofstream fs(path);
        fs.write(data.data(), static_cast<std::streamsize>(data.size()));
        fs.close();
    }

    stream = std::ifstream("assets_encrypted.pak", std::ios_base::in | std::ios::binary);
    pak = xng::Pak(stream, *zip, *sha, *aes, "test");
    for (auto &pair: pak.getEntries()) {
        auto data = pak.get(pair.first, true);
        auto path = "extract_encrypted/" + pair.first;
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        std::ofstream fs(path);
        fs.write(data.data(), static_cast<std::streamsize>(data.size()));
        fs.close();
    }
}