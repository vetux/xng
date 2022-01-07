#ifndef MANA_PAK_HPP
#define MANA_PAK_HPP

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <map>

#include "crypto/aes.hpp"

namespace engine {
    static const std::string PAK_FORMAT_VERSION = "00";
    static const std::string PAK_HEADER_MAGIC = "\xa9pak\xff" + PAK_FORMAT_VERSION + "\xa9";

    /**
     * The pak file format
     */
    class MANA_EXPORT Pak {
    public:
        struct HeaderEntry {
            size_t offset;
            size_t size;
            std::string hash;
        };

        static std::map<std::string, std::vector<char>> readEntries(const std::string &path, bool recursive = true);

        /**
         * Create a pak buffer from the passed entries and return it.
         *
         * @param entries
         * @param chunkSize The maximum number of bytes stored in a single pak chunk
         * @return The ordered pak chunks
         */
        static std::vector<std::vector<char>> createPak(const std::map<std::string, std::vector<char>> &entries,
                                                        long chunkSize = -1,
                                                        bool compressData = true);

        /**
         * Create a pak buffer from the passed entries and return it.
         * Additionally apply aes encryption to the asset data using the supplied key and initialization vector.
         *
         * @param entries
         * @param chunkSize
         * @param key
         * @param iv
         * @return The ordered pak chunks
         */
        static std::vector<std::vector<char>> createPak(const std::map<std::string, std::vector<char>> &entries,
                                                        long chunkSize,
                                                        bool compressData,
                                                        const AES::Key &key,
                                                        const AES::InitializationVector &iv);

        Pak() = default;

        /**
         * Load a pak buffer from stream.
         *
         * @param streams
         * @param key
         * @param iv
         */
        explicit Pak(std::vector<std::unique_ptr<std::istream>> streams,
                     AES::Key key = {},
                     AES::InitializationVector iv = {});

        /**
         * Load the pak entry from the stream, and optionally verify its hash.
         *
         * @param path The path of the entry
         * @param verifyHash If true the hash of the returned data is checked against a hash stored in the pak header and an exception is thrown on mismatch.
         * @return The entry data
         */
        std::vector<char> get(const std::string &path, bool verifyHash = false);

        bool exists(const std::string &path) {
            return entries.find(path) != entries.end();
        }

    private:
        void loadHeader();

        size_t getRelativeOffset(size_t globalOffset);

        std::istream &getStreamForOffset(size_t globalOffset);

        std::vector<std::unique_ptr<std::istream>> streams;
        std::map<std::string, HeaderEntry> entries; // The header entries with global offsets
        long chunkSize{};
        bool encrypted{};
        bool compressed{};
        AES::Key key{};
        AES::InitializationVector iv{};
    };
}

#endif //MANA_PAK_HPP
