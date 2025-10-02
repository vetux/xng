/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_RESOURCEIMPORTER_HPP
#define XENGINE_RESOURCEIMPORTER_HPP

#include <string>
#include <vector>

#include "xng/resource/resourcebundle.hpp"
#include "xng/resource/uri.hpp"

#include "xng/io/archive.hpp"

namespace xng {
    /**
     * The resource importer handles importing of resources from streams pointing to resource data.
     */
    class XENGINE_EXPORT ResourceImporter {
    public:
        virtual ~ResourceImporter() = default;

        /**
         * Import the bundle from the stream.
         *
         * If the bundle format references other bundles by path (For example fbx) the importer uses the archive instance
         * to resolve the paths, and imports the resolved data using this->import().
         *
         * @param stream The stream to the data to be imported.
         * @param path The path of the resource to import.
         * @param archive The archive instance to use when resolving paths in the stream data.
         * @return
         */
        virtual ResourceBundle read(std::istream &stream, const Uri &path, Archive *archive) = 0;

        /**
         * @return The set of supported file extensions with each containing the preceding dot
         */
        virtual const std::set<std::string> &getSupportedFormats() const = 0;
    };
}

#endif //XENGINE_RESOURCEIMPORTER_HPP
