/**
 *  XEngine - C++ game engine library
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

#include "io/pak.hpp"

#include <utility>
#include <filesystem>

#include "extern/json.hpp"
#include "extern/base64.hpp"

#include "io/readfile.hpp"
#include "compression/gzip.hpp"
#include "crypto/sha.hpp"

namespace xengine {
    std::map<std::string, std::vector<char>> Pak::readEntries(const std::string &directory, bool recursive) {
        std::map<std::string, std::vector<char>> ret;
        for (auto &file: std::filesystem::recursive_directory_iterator(directory)) {
            if (file.is_regular_file()) {
                auto path = "/" + std::filesystem::relative(file.path(), directory).string();
                auto data = readFile(file.path().string());
                ret[path] = data;
            }
        }
        return ret;
    }

    static std::vector<std::vector<char>> createPakInternal(const std::map<std::string, std::vector<char>> &entries,
                                                            long chunkSize,
                                                            bool compress,
                                                            bool encrypt,
                                                            const AES::Key &key,
                                                            const AES::InitializationVector &iv) {
        std::vector<char> data;
        size_t currentOffset = 0;

        std::map<std::string, Pak::HeaderEntry> headerEntries;
        for (auto &pair: entries) {
            auto d = pair.second;

            if (compress) {
                d = GZip::compress(d);
            }

            if (encrypt) {
                d = AES::encrypt(key, iv, d);
            }

            headerEntries[pair.first].offset = currentOffset;
            headerEntries[pair.first].size = d.size();
            headerEntries[pair.first].hash = SHA::sha256(pair.second);

            currentOffset += d.size();

            data.insert(data.begin() + static_cast<long>(data.size()), d.begin(), d.end());
        }

        nlohmann::json headerJson;
        headerJson["chunkSize"] = chunkSize;
        headerJson["compressed"] = compress;
        for (auto &pair: headerEntries) {
            auto &element = headerJson["entries"][pair.first];
            element["offset"] = pair.second.offset;
            element["size"] = pair.second.size;
            element["hash"] = pair.second.hash;
        }

        auto headerStr = headerJson.dump();
        headerStr = GZip::compress(headerStr);

        if (encrypt) {
            headerStr = AES::encrypt(key, iv, headerStr);
        }

        nlohmann::json outHeaderJson;
        outHeaderJson["encrypted"] = encrypt;
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

    std::vector<std::vector<char>> Pak::createPak(const std::map<std::string, std::vector<char>> &entries,
                                                  long chunkSize,
                                                  bool compressData) {
        return createPakInternal(entries, chunkSize, compressData, false, {}, {});
    }

    std::vector<std::vector<char>> Pak::createPak(const std::map<std::string, std::vector<char>> &entries,
                                                  long chunkSize,
                                                  bool compressData,
                                                  const AES::Key &key,
                                                  const AES::InitializationVector &iv) {
        return createPakInternal(entries, chunkSize, compressData, true, key, iv);
    }

    Pak::Pak(std::vector<std::unique_ptr<std::istream>> streams, AES::Key key, AES::InitializationVector iv)
            : streams(std::move(streams)),
              key(std::move(key)),
              iv(iv) {
        loadHeader();
    }

    std::vector<char> Pak::get(const std::string &path, bool verifyHash) {
        std::vector<char> ret;
        auto hEntry = entries.at(path);

        /*
        // Simple but slow, read each character sequentially.
        for (auto i = 0; i < hEntry.size; i++) {
            auto pos = hEntry.offset + i;
            auto relOff = getRelativeOffset(pos); // The offset relative to the stream
            auto &stream = getStreamForOffset(pos); // The stream which provides the offset
            stream.seekg(static_cast<std::streamoff>(relOff));
            stream.read(&ret.at(i), static_cast<std::streamoff>(1));
            if (stream.gcount() != 1)
                throw std::runtime_error("Invalid pak entry data length");
        }
        */

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

        if (encrypted) {
            ret = AES::decrypt(key, iv, ret);
        }

        if (compressed) {
            ret = GZip::decompress(ret);
        }

        if (verifyHash) {
            auto hash = SHA::sha256(ret);
            if (hEntry.hash != hash) {
                throw std::runtime_error("Pak entry data hash mismatch");
            }
        }

        return ret;
    }

    void Pak::loadHeader() {
        size_t dataBegin;
        std::string headerStr;
        int scope = -1;
        char c;
        int streamIndex = 0;
        auto calChunkSize = 0;
        for (auto i = 0; i < std::numeric_limits<size_t>::max(); i++) {
            auto &s = *streams.at(streamIndex);
            auto offset = i - ((calChunkSize) * streamIndex);

            s.seekg(static_cast<std::streamoff>(offset));
            s.read(&c, 1);

            if (s.eof()) {
                if (calChunkSize == 0)
                    calChunkSize = i;

                streamIndex++;

                offset = i - ((calChunkSize) * streamIndex);

                auto &tmp = *streams.at(streamIndex);
                tmp.seekg(static_cast<std::streamoff>(offset));
                tmp.read(&c, 1);
            } else if (s.gcount() == 0) {
                throw std::runtime_error("Failed to load header (Invalid length)");
            }

            if (i < PAK_HEADER_MAGIC.size())
                continue;

            headerStr += c;

            if (c == '{') {
                if (scope == -1)
                    scope = 1;
                else
                    scope++;
            } else if (c == '}') {
                scope--;
            }

            if (scope == 0) {
                dataBegin = i + 1;
                break;
            }
        }

        auto headerWrap = nlohmann::json::parse(headerStr);

        encrypted = headerWrap["encrypted"];

        headerStr = base64_decode(static_cast<std::string>(headerWrap["data"]));

        if (encrypted) {
            try {
                headerStr = xengine::AES::decrypt(key, {}, headerStr);
            } catch (const std::exception &e) {
                std::string error = "Failed to decrypt pak header (Wrong Key?): " + std::string(e.what());
                throw std::runtime_error(error);
            }
        }

        headerStr = xengine::GZip::decompress(headerStr);

        auto headerJson = nlohmann::json::parse(headerStr);

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
            return *streams.at(0);
        auto nChunk = globalOffset / chunkSize;
        return *streams.at(nChunk);
    }
}