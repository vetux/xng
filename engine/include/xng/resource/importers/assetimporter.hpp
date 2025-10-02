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

#ifndef XENGINE_ASSETIMPORTER_HPP
#define XENGINE_ASSETIMPORTER_HPP

#include "xng/resource/resourceimporter.hpp"

#include "xng/io/message.hpp"

namespace xng {
    // TODO: Implement custom asset file format
    /**
     * Import assets from a custom file format for the engine.
     *
     * Because assimp binary footprint is big, requires rtti and imports from generic formats such as fbx are slow,
     * I will design a custom file format for storing assets to be consumed by the engine at runtime.
     *
     * The editor will work with the generic file formats imported using assimp and export the assets to the custom file format on build.
     */
    class XENGINE_EXPORT AssetImporter final : public ResourceImporter {
    public:
        ResourceBundle read(std::istream &stream,
                            const Uri &path,
                            Archive *archive) override;

        const std::set<std::string> &getSupportedFormats() const override;
    };
}

#endif //XENGINE_ASSETIMPORTER_HPP