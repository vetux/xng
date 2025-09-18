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

#include "xng/resource/importers/stbiimporter.hpp"

#include "thirdparty/stb_image.h"
#include "xng/render/scene/image.hpp"
#include "xng/render/scene/texture.hpp"

#include <cstring>

namespace xng {
    static ImageRGBA readImage(const std::vector<char> &buffer) {
        int width, height, nrChannels;
        stbi_uc *data = stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(buffer.data()),
                                              buffer.size(),
                                              &width,
                                              &height,
                                              &nrChannels,
                                              4);
        if (data) {
            auto ret = ImageRGBA(width, height);
            std::memcpy(reinterpret_cast<stbi_uc *>(ret.getBuffer().data()),
                        data,
                        ret.getBuffer().size() * sizeof(ColorRGBA));
            stbi_image_free(data);
            return ret;
        } else {
            stbi_image_free(data);
            std::string error = "Failed to load image";
            throw std::runtime_error(error);
        }
    }

    ResourceBundle StbiImporter::read(std::istream &stream,
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
        //Try to read source as image
        int x, y, n;
        auto r = stbi_info_from_memory((const stbi_uc *) (buffer.data()),
                                       buffer.size(),
                                       &x,
                                       &y,
                                       &n);
        if (r == 1) {
            //Source is image
            ResourceBundle ret;
            auto img = readImage(buffer);
            ret.add("image", std::make_unique<ImageRGBA>(img));
            Texture tex;
            tex.image = ResourceHandle<ImageRGBA>(Uri(path));
            tex.description.size = img.getResolution();
            ret.add("imageTexture", std::make_unique<Texture>(tex));
            return ret;
        } else {
            throw std::runtime_error("Failed to read image info: " + std::string(stbi_failure_reason()));
        }
    }

    const std::set<std::string> &StbiImporter::getSupportedFormats() const {
        static const std::set<std::string> formats = {".png", ".jpeg", ".jpg", ".bmp", ".tga", ".gif", ".hdr"};
        return formats;
    }
}