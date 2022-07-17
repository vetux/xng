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

#include "resource/parsers/stbiparser.hpp"

#include "extern/stb_image.h"
#include "asset/image.hpp"

#include <cstring>

namespace xng {
    static ImageRGBA readImage(const std::string &buffer) {
        int width, height, nrChannels;
        stbi_uc *data = stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(buffer.data()),
                                              buffer.size(),
                                              &width,
                                              &height,
                                              &nrChannels,
                                              4);
        if (data) {
            auto ret = ImageRGBA(width, height);
            std::memcpy(ret.getData(), data, (width * height) * (sizeof(stbi_uc) * 4));
            stbi_image_free(data);
            return ret;
        } else {
            stbi_image_free(data);
            std::string error = "Failed to load image";
            throw std::runtime_error(error);
        }
    }

    ResourceBundle StbiParser::read(const std::string &buffer,
                                    const std::string &hint,
                                    const ResourceImporter &importer,
                                    Archive *archive) const {
        //Try to read source as image
        int x, y, n;
        if (stbi_info_from_memory(reinterpret_cast<const stbi_uc *>(buffer.data()),
                                  buffer.size(),
                                  &x,
                                  &y,
                                  &n) == 1) {
            //Source is image
            ResourceBundle ret;
            ret.add("", std::make_unique<ImageRGBA>(readImage(buffer)));
            return ret;
        } else {
            throw std::runtime_error("Failed to read image info");
        }
    }

    const std::set<std::string> &StbiParser::getSupportedFormats() const {
        static const std::set<std::string> formats = {"png", "jpeg", "gif"};
        return formats;
    }
}