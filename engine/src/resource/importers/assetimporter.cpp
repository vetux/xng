/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "xng/resource/importers/assetimporter.hpp"

namespace xng {
    // TODO: Implement custom asset file format

    ResourceBundle AssetImporter::read(std::istream &stream, const Uri &path, Archive *archive) {
        throw std::runtime_error("Not implemented");
    }

    const std::set<std::string> & AssetImporter::getSupportedFormats() const {
        throw std::runtime_error("Not implemented");
    }
}
