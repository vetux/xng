/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include <filesystem>

#include "resource/parsers/assimpparser.hpp"
#include "resource/parsers/jsonparser.hpp"
#include "resource/parsers/sndfileparser.hpp"
#include "resource/parsers/stbiparser.hpp"

namespace xng {

    ResourceImporter::ResourceImporter()
            : parsers() {
        parsers.emplace_back(std::make_unique<AssImpParser>());
        parsers.emplace_back(std::make_unique<StbiParser>());
        parsers.emplace_back(std::make_unique<SndFileParser>());
        parsers.emplace_back(std::make_unique<JsonParser>());
    }

    ResourceImporter::ResourceImporter(std::vector<std::unique_ptr<ResourceParser>> parsers)
            : parsers(std::move(parsers)) {}

    ResourceBundle ResourceImporter::import(std::istream &stream, const std::string &hint, Archive *archive) {
        std::string buffer = {std::istreambuf_iterator<char>(stream), {}};

        // Use parser which supports hint if exists.
        for (auto &parser: parsers) {
            auto formats = parser->getSupportedFormats();
            if (formats.find(hint) != formats.end()) {
                return parser->parse(buffer, hint, archive);
            }
        }

        // Otherwise try every parser
        for (auto &parser: parsers) {
            try {
                return parser->parse(buffer, hint, archive);
            } catch (const std::exception &e) {}
        }
        throw std::runtime_error("Failed to read bundle");
    }
}