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

#include "xng/io/pak.hpp"

#include <utility>
#include <filesystem>

#include "thirdparty/json.hpp"
#include "thirdparty/base64.hpp"

#include "xng/io/readfile.hpp"
#include "xng/crypto/gzip.hpp"
#include "xng/crypto/sha.hpp"

namespace xng {
    Pak::Pak(std::vector<std::reference_wrapper<std::istream>> streams, GZip &gzip, SHA &sha)
            : streams(std::move(streams)), gzip(&gzip), sha(&sha) {
        loadHeader();
    }

    Pak::Pak(std::vector<std::reference_wrapper<std::istream>> streams,
             GZip &gzip,
             SHA &sha,
             AES &aes,
             AES::Key key)
            : streams(std::move(streams)),
              key(std::move(key)),
              gzip(&gzip),
              sha(&sha),
              aes(&aes) {
        loadHeader();
    }

    std::vector<char> Pak::get(const std::string &path, bool verifyHash) {
        std::vector<char> ret;
        auto hEntry = entries.at(path);

        if (chunkSize == 0) {
            auto &stream = getStreamForOffset(hEntry.offset);
            std::vector<char> tmp(hEntry.size);
            stream.seekg(static_cast<std::streamoff>(hEntry.offset));
            stream.read(tmp.data(), static_cast<std::streamoff>(hEntry.size));
            if (stream.gcount() != hEntry.size)
                throw std::runtime_error("Failed to read pak entry");
            ret.insert(ret.begin(), tmp.begin(), tmp.end());
        } else {
            auto beginOffset = hEntry.offset % chunkSize; // The offset into the first chunk stream
            auto beginCount = chunkSize - beginOffset; // The number of bytes in the first chunk stream

            if (hEntry.size < beginCount) {
                beginCount = hEntry.size;
            }

            auto remainingCount = static_cast<int>(hEntry.size - beginCount); // The number of remaining bytes

            auto remainChunks = remainingCount / chunkSize; // The number of full chunks that the remaining bytes fill

            auto lastCount = remainingCount %
                             chunkSize; // The number of bytes in the last partial chunk stream. If the data size matches chunk size a "middle" chunk is the end.

            // Read first chunk
            if (beginCount > 0) {
                auto &firstStream = getStreamForOffset(hEntry.offset);
                std::vector<char> tmp(beginCount);
                firstStream.seekg(static_cast<std::streamoff>(beginOffset));
                firstStream.read(tmp.data(), static_cast<std::streamoff>(beginCount));
                if (firstStream.gcount() != beginCount)
                    throw std::runtime_error("Failed to read pak entry");
                ret.insert(ret.begin(), tmp.begin(), tmp.end());
            }

            if (remainChunks > 0) {
                //Read full chunks
                for (int i = 0; i < remainChunks; i++) {
                    std::vector<char> chunk(chunkSize);
                    auto gOffset = hEntry.offset + beginCount + (i * chunkSize);
                    auto &stream = getStreamForOffset(gOffset);
                    stream.seekg(0);
                    stream.read(chunk.data(), static_cast<std::streamoff>(chunk.size()));
                    if (stream.gcount() != chunk.size())
                        throw std::runtime_error("Failed to read pak chunk");
                    ret.insert(ret.end(), chunk.begin(), chunk.end());
                }
            }

            if (lastCount > 0) {
                // Read last partial chunk
                std::vector<char> lastChunk(lastCount);
                auto gOffset = hEntry.offset + beginCount + (remainChunks * chunkSize);
                auto &lastStream = getStreamForOffset(gOffset);
                lastStream.seekg(0);
                lastStream.read(lastChunk.data(), static_cast<std::streamoff>(lastChunk.size()));

                if (lastStream.gcount() != lastChunk.size())
                    throw std::runtime_error("Failed to read pak chunk");

                ret.insert(ret.begin()
                           + static_cast<std::streamoff>(beginCount)
                           + static_cast<std::streamoff>(remainChunks) * chunkSize,
                           lastChunk.begin(),
                           lastChunk.end());
            }
        }

        if (encrypted) {
            ret = aes->decrypt(key, iv, ret);
        }

        if (compressed) {
            ret = gzip->decompress(ret);
        }

        if (verifyHash) {
            auto hash = sha->sha256(ret);
            if (hEntry.hash != hash) {
                throw std::runtime_error("Pak entry data hash mismatch");
            }
        }

        return ret;
    }

    void Pak::loadHeader() {
        size_t dataBegin{};
        std::string headerStr{};
        std::string headerSizeStr{};
        size_t headerSize{};
        int scope = -1;
        char c;
        int streamIndex = 0;
        auto calChunkSize = 0;
        for (auto i = 0; i < std::numeric_limits<size_t>::max(); i++) {
            auto &s = streams.at(streamIndex).get();
            auto offset = i - ((calChunkSize) * streamIndex);

            s.seekg(static_cast<std::streamoff>(offset));
            s.read(&c, 1);

            if (s.eof()) {
                if (streams.size() < streamIndex + 1) {
                    throw std::runtime_error("Failed to load header (End of file)");
                }

                if (calChunkSize == 0)
                    calChunkSize = i;

                streamIndex++;

                offset = i - ((calChunkSize) * streamIndex);

                auto &tmp = streams.at(streamIndex).get();
                tmp.seekg(static_cast<std::streamoff>(offset));
                tmp.read(&c, 1);
            } else if (s.gcount() == 0) {
                throw std::runtime_error("Failed to load header (End of file gcount)");
            }

            if (i < PAK_HEADER_MAGIC.size())
                continue;

            if (c == '\xa7' && scope != 2) {
                if (scope == -1) {
                    scope = 1;
                } else {
                    headerSize = std::stoul(headerSizeStr);
                    dataBegin = i + 1 + headerSize;
                    scope = 2;
                }
            } else if (scope == 1) {
                headerSizeStr += c;
            } else if (scope == 2) {
                headerStr += c;
                if (headerStr.size() == headerSize)
                    break;
            }
        }

        auto headerJson = nlohmann::json::from_bson(headerStr);
        if (headerJson.contains("edata")) {
            encrypted = true;

            auto decodedHeader = static_cast<std::string>(headerJson["edata"]);

            auto ivStr = headerJson["iv"].get<std::string>();
            for (auto i = 0; i < ivStr.size() && i < iv.size(); i++) {
                iv[i] = ivStr[i];
            }

            if (aes == nullptr) {
                throw std::runtime_error("Pak data is encrypted but no aes implementation was specified.");
            }

            try {
                decodedHeader = aes->decrypt(key, iv, decodedHeader);
            } catch (const std::exception &e) {
                std::string error = "Failed to decrypt pak header (Wrong Key?): " + std::string(e.what());
                throw std::runtime_error(error);
            }

            headerJson = nlohmann::json::parse(decodedHeader);
        }

        compressed = headerJson["compressed"];
        chunkSize = headerJson["chunkSize"];

        for (auto &pair: headerJson.value<std::map<std::string, nlohmann::json>>("entries", {})) {
            auto &path = pair.first;
            auto &entry = pair.second;
            size_t offset = entry["offset"];
            size_t size = entry["size"];
            std::string hash = entry["hash"];
            entries[path] = {dataBegin + offset, size, hash};
        }
    }

    size_t Pak::getRelativeOffset(size_t globalOffset) {
        if (chunkSize <= 0)
            return globalOffset;

        if (globalOffset < chunkSize) {
            return globalOffset;
        } else {
            auto rChunk = globalOffset % chunkSize; // The relative offset ( Remainder )
            return rChunk;
        }
    }

    std::istream &Pak::getStreamForOffset(size_t globalOffset) {
        if (chunkSize <= 0)
            return streams.at(0);
        auto nChunk = globalOffset / chunkSize;
        return streams.at(nChunk);
    }
}