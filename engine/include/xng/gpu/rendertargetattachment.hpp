/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_RENDERTARGETATTACHMENT_HPP
#define XENGINE_RENDERTARGETATTACHMENT_HPP

#include <variant>

#include "xng/gpu/textureproperties.hpp"

namespace xng {
    class TextureBuffer;

    class TextureArrayBuffer;

    enum AttachmentType {
        ATTACHMENT_TEXTURE = 0,
        ATTACHMENT_CUBEMAP = 1,
        ATTACHMENT_TEXTUREARRAY = 2
    };

    typedef std::variant<std::reference_wrapper<TextureBuffer>,
            std::pair<CubeMapFace, std::reference_wrapper<TextureBuffer>>,
            std::pair<size_t, std::reference_wrapper<TextureArrayBuffer>>> RenderTargetAttachment;
}

#endif //XENGINE_RENDERTARGETATTACHMENT_HPP
