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

namespace xng {
    /**
     * The resource importer is the abstraction around resource formats such as fbx, obj, mp3, png etc.
     */
    class XENGINE_EXPORT ResourceImporter {
    public:
        ResourceImporter() = default;

        /**
         * Import the bundle from the stream.
         *
         * If the bundle format references other bundles (For example fbx) the importer uses the archive instance
         * to resolve the paths, and throws if a path could not be resolved.
         *
         * @param stream
         * @param hint The file extension
         * @param archive
         * @return
         */
        ResourceBundle import(std::istream &stream, const std::string &hint = "", Archive *archive = nullptr);
    };
}

#endif //XENGINE_RESOURCEIMPORTER_HPP
