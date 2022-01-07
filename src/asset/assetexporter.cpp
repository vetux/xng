/**
 *  Mana - 3D Game Engine
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

#include "asset/assetexporter.hpp"

#include "extern/stb_image_write.h"

void streamWriteFunc(void *context, void *data, int size) {
    auto &stream = *static_cast<std::ostream *>(context);
    stream.write(static_cast<char *>(data), size);
}

namespace xengine {
    void AssetExporter::exportImage(std::ostream &stream, const Image<ColorRGBA> &image) {
        int r = stbi_write_png_to_func(&streamWriteFunc,
                                       &stream,
                                       image.getWidth(),
                                       image.getHeight(),
                                       4,
                                       image.getData(),
                                       image.getWidth() * 4);
        if (r != 1) {
            throw std::runtime_error("Failed to write image");
        }
    }
}