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

#ifndef XENGINE_RESOURCEIMPORTER_HPP
#define XENGINE_RESOURCEIMPORTER_HPP

#include <string>
#include <vector>

#include "resource/resourcebundle.hpp"
#include "resource/resourceparser.hpp"

#include "io/archive.hpp"

namespace xng {
    /**
     * The resource importer handles importing of resources from streams pointing to resource data.
     * The set resource parsers define the formats that the resource importer can import.
     *
     * It handles a set of parsers and decides which parsers to invoke for given inputs.
     */
    class XENGINE_EXPORT ResourceImporter {
    public:
        ResourceImporter();

        /**
         * @param parsers The set of parsers to use when importing data.
         */
        explicit ResourceImporter(std::vector<std::unique_ptr<ResourceParser>> parsers);

        /**
         * Import the bundle from the stream.
         *
         * If the bundle format references other bundles by path (For example fbx) the importer uses the archive instance
         * to resolve the paths, and imports the resolved data using this->import().
         *
         * @param stream The stream to the data to be imported.
         * @param hint The file extension
         * @param archive The archive instance to use when resolving paths in the stream data.
         * @return
         */
        ResourceBundle import(std::istream &stream, const std::string &hint = "", Archive *archive = nullptr) const;

    private:
        std::vector<std::unique_ptr<ResourceParser>> parsers;
    };
}

#endif //XENGINE_RESOURCEIMPORTER_HPP
