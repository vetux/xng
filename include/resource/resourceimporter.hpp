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

#ifndef XENGINE_RESOURCEIMPORTER_HPP
#define XENGINE_RESOURCEIMPORTER_HPP

#include <string>
#include <vector>

#include "asset/material.hpp"
#include "asset/audio.hpp"
#include "resource/resourcebundle.hpp"

#include "async/threadpool.hpp"
#include "io/archive.hpp"

namespace xengine {
    class XENGINE_EXPORT ResourceImporter {
    public:
        ResourceImporter() = default;

        /**
         * Import the bundle from the stream.
         *
         * If the bundle format references other bundles and nullptr archive is passed a exception is thrown.
         *
         * @param stream
         * @param hint
         * @param archive
         * @return
         */
        ResourceBundle import(std::istream &stream, const std::string &hint = "", Archive *archive = nullptr);

        /**
         * Import the bundle from the path.
         *
         * The bundle path is resolved using the passed archive instance.
         *
         * If the bundle format references other bundles by name they are resolved using the
         * passed archive instance.
         *
         * @param stream
         * @param archive
         * @return
         */
        ResourceBundle import(const std::string &path, Archive &archive);
    };
}

#endif //XENGINE_RESOURCEIMPORTER_HPP
