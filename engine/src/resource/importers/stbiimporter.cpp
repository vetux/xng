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

#include "xng/resource/importers/stbiimporter.hpp"

#include "thirdparty/stb_image.h"
#include "xng/graphics/image.hpp"
#include "xng/graphics/scene/texture.hpp"

#include <cstring>

namespace xng {
    ImageRGBA StbiImporter::readImageRGBA(const std::vector<char> &buffer) {
        int width, height, nrChannels;
        stbi_uc *data = stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(buffer.data()),
                                              static_cast<int>(buffer.size()),
                                              &width,
                                              &height,
                                              &nrChannels,
                                              4);
        if (data) {
            auto ret = ImageRGBA(width, height);
            std::memcpy(ret.getBuffer().data(),
                        data,
                        ret.getBuffer().size() * sizeof(ColorRGBA));
            stbi_image_free(data);
            return ret;
        }

        stbi_image_free(data);

        std::string error = "Failed to load image";

        const auto reason = stbi_failure_reason();
        if (reason) {
            error += ": " + std::string(reason);
        }

        throw std::runtime_error(error);
    }

    ImageFloat StbiImporter::readImageFloat(const std::vector<char> &buffer) {
        int width, height, nrChannels;
        float *data = stbi_loadf_from_memory(reinterpret_cast<const stbi_uc *>(buffer.data()),
                                             static_cast<int>(buffer.size()),
                                             &width,
                                             &height,
                                             &nrChannels,
                                             4);
        if (data) {
            auto ret = Image<float>(width, height);
            std::memcpy(ret.getBuffer().data(),
                        data,
                        ret.getBuffer().size() * sizeof(float));
            stbi_image_free(data);
            return ret;
        }

        stbi_image_free(data);

        std::string error = "Failed to load image";

        const auto reason = stbi_failure_reason();
        if (reason) {
            error += ": " + std::string(reason);
        }

        throw std::runtime_error(error);
    }

    ResourceBundle StbiImporter::read(std::istream &stream,
                                      const Uri &path,
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
        auto r = stbi_info_from_memory(reinterpret_cast<const stbi_uc *>(buffer.data()),
                                       buffer.size(),
                                       &x,
                                       &y,
                                       &n);
        if (r == 1) {
            //Source is image
            ResourceBundle ret;
            if (path.getExtension() == "hdr") {
                auto img = readImageFloat(buffer);
                ret.add("image", std::make_unique<Image<float> >(img));
            } else {
                auto img = readImageRGBA(buffer);
                ret.add("image", std::make_unique<ImageRGBA>(img));
                Texture tex;
                tex.filter = Texture::NEAREST;
                tex.image = ResourceHandle<ImageRGBA>(Uri(path));
                ret.add("texture", std::make_unique<Texture>(tex));
            }
            return ret;
        }

        std::string error = "Failed to read image info";

        const auto reason = stbi_failure_reason();
        if (reason) {
            error += ": " + std::string(reason);
        }

        throw std::runtime_error(error);
    }

    const std::set<std::string> &StbiImporter::getSupportedFormats() const {
        static const std::set<std::string> formats = {".png", ".jpeg", ".jpg", ".bmp", ".tga", ".gif", ".hdr", ".psd"};
        return formats;
    }
}
