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

#include "resource/resourceimporter.hpp"

#include "asset/rawasset.hpp"

namespace xng {

    ResourceImporter::ResourceImporter()
            : parsers() {}

    ResourceImporter::ResourceImporter(std::vector<std::unique_ptr<ResourceParser>> parsers)
            : parsers(std::move(parsers)) {}

    ResourceBundle ResourceImporter::import(std::istream &stream, const std::string &hint, Archive *archive) const {
        std::vector<char> buffer;

        char c;
        while (!stream.eof()) {
            stream.read(&c, 1);
            if (stream.gcount() == 1) {
                buffer.emplace_back(c);
            }
        }

        // Use parser which supports hint if it exists.
        for (auto &parser: parsers) {
            auto formats = parser->getSupportedFormats();
            if (formats.find(hint) != formats.end()) {
                return parser->read(buffer, hint, *this, archive);
            }
        }

        // Import as raw
        ResourceBundle ret;
        RawAsset asset;
        asset.bytes = std::vector<uint8_t>(buffer.begin(), buffer.end());
        ret.add("0", std::make_unique<RawAsset>(asset));
        return ret;
    }
}
