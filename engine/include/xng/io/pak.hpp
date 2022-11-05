#ifndef XENGINE_PAK_HPP
#define XENGINE_PAK_HPP

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <map>

#include "xng/crypto/aes.hpp"
#include "xng/crypto/gzip.hpp"
#include "xng/crypto/sha.hpp"

namespace xng {
    static const std::string PAK_FORMAT_VERSION = "00";
    static const std::string PAK_HEADER_MAGIC = "\xa9pak\xff" + PAK_FORMAT_VERSION + "\xa9";

    /**
     * The pak file format
     */
    class XENGINE_EXPORT Pak {
    public:
        struct HeaderEntry {
            size_t offset;
            size_t size;
            std::string hash;
        };

        Pak() = default;

        Pak(std::vector<std::reference_wrapper<std::istream>> streams, GZip &gzip, SHA &sha);

        /**
         * @param streams The chunk streams in the order returned by PakBuilder::build
         * @param key The key used to decrypt encrypted entries
         * @param iv The iv used to decrypt encrypted entries
         */
        Pak(std::vector<std::reference_wrapper<std::istream>> streams,
            GZip &gzip,
            SHA &sha,
            AES &aes,
            AES::Key key,
            AES::InitializationVector iv);

        /**
         * Load the pak entry from the corresponding chunk stream,
         * and optionally verify its hash.
         *
         * The data of the entry is loaded into memory from the chunk streams when this method is called.
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

        std::vector<std::reference_wrapper<std::istream>> streams;
        std::map<std::string, HeaderEntry> entries; // The header entries with global offsets
        long chunkSize{};
        bool encrypted{};
        bool compressed{};

        AES *aes = nullptr;
        AES::Key key{};
        AES::InitializationVector iv{};

        GZip *gzip = nullptr;
        SHA *sha = nullptr;
    };
}

#endif //XENGINE_PAK_HPP
