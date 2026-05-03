/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <sstream>

#include "xng/resource/importers/fontimporter.hpp"

#include "xng/assets/font.hpp"

namespace xng {
    ResourceBundle FontImporter::read(std::istream &stream,
                                      const Uri &path,
                                      Archive *archive) {
        std::vector<uint8_t> buffer;

        char c;
        while (!stream.eof()) {
            stream.read(&c, 1);
            if (stream.gcount() == 1) {
                buffer.emplace_back(c);
            }
        }

        ResourceBundle ret;
        ret.add("", std::make_unique<Font>(Font(buffer)));
        return ret;
    }

    const std::set<std::string> &FontImporter::getSupportedFormats() const {
        static const std::set<std::string> formats = {".ttf"};
        return formats;
    }
}