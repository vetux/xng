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

#ifndef XENGINE_STBIPARSER_HPP
#define XENGINE_STBIPARSER_HPP

#include "xng/graphics/image.hpp"
#include "xng/resource/resourceimporter.hpp"

namespace xng {
    class XENGINE_EXPORT StbiImporter : public ResourceImporter {
    public:
        static ImageRGBA readImage(const std::vector<char> &buffer);

        ResourceBundle read(std::istream &stream,
                            const Uri &path,
                            Archive *archive) override;

        const std::set<std::string> &getSupportedFormats() const override;
    };
}

#endif //XENGINE_STBIPARSER_HPP
