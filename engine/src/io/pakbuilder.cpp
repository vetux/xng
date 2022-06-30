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

#include "io/pakbuilder.hpp"

#include "extern/json.hpp"
#include "extern/base64.hpp"
#include "compression/gzip.hpp"
#include "crypto/sha.hpp"

namespace xng {
    void PakBuilder::addEntry(const std::string &name, const std::vector<char> &buffer) {
        if (entries.find(name) != entries.end())
            throw std::runtime_error("Entry with name " + name + " already exists");
        entries[name] = buffer;
    }

    std::vector<std::vector<char>> PakBuilder::build(size_t chunkSize,
                                                     bool compressData,
                                                     bool encryptData,
                                                     const AES::Key &key,
                                                     const AES::InitializationVector &iv) {
        std::vector<char> data;
        size_t currentOffset = 0;

        std::map<std::string, Pak::HeaderEntry> headerEntries;
        for (auto &pair: entries) {
            auto d = pair.second;

            if (compressData) {
                d = GZip::compress(d);
            }

            if (encryptData) {
                d = AES::encrypt(key, iv, d);
            }

            headerEntries[pair.first].offset = currentOffset;
            headerEntries[pair.first].size = d.size();
            headerEntries[pair.first].hash = SHA::sha256(pair.second);

            currentOffset += d.size();

            data.insert(data.end(), d.begin(), d.end());
        }

        nlohmann::json headerJson;
        headerJson["chunkSize"] = chunkSize;
        headerJson["compressed"] = compressData;
        for (auto &pair: headerEntries) {
            auto &element = headerJson["entries"][pair.first];
            element["offset"] = pair.second.offset;
            element["size"] = pair.second.size;
            element["hash"] = pair.second.hash;
        }

        auto headerStr = headerJson.dump();

        if (encryptData) {
            headerStr = AES::encrypt(key, iv, headerStr);
        }

        nlohmann::json outHeaderJson;
        outHeaderJson["encrypted"] = encryptData;
        outHeaderJson["data"] = base64_encode(headerStr);

        auto outHeaderStr = outHeaderJson.dump();

        auto hdr = PAK_HEADER_MAGIC + outHeaderStr;

        if (chunkSize > 0) {
            auto totalSize = hdr.size() + data.size();
            auto totalWholeChunks = totalSize / chunkSize;
            auto totalRemainder = totalSize % chunkSize;

            auto totalChunks = totalWholeChunks;
            if (totalRemainder != 0)
                totalChunks += 1;

            std::vector<std::vector<char>> ret(totalChunks);

            size_t chunkIndex = 0;
            for (auto i = 0; i < totalSize; i++) {
                auto &chunk = ret.at(chunkIndex);

                if (i < hdr.size()) {
                    chunk.emplace_back(hdr.at(i));
                } else {
                    chunk.emplace_back(data.at(i - hdr.size()));
                }

                if (chunk.size() == chunkSize)
                    chunkIndex++;
            }

            return ret;
        } else {
            data.insert(data.begin(), hdr.begin(), hdr.end());
            return {data};
        }
    }
}