/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include <sstream>

#include "xng/resource/importers/fontimporter.hpp"

#include "xng/font/font.hpp"

namespace xng {
    ResourceBundle FontImporter::read(std::istream &stream,
                                      const std::string &hint,
                                      const std::string &path,
                                      Archive *archive) {
        std::vector<char> buffer;

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