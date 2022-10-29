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

#ifndef XENGINE_FRAMEGRAPHALLOCATION_HPP
#define XENGINE_FRAMEGRAPHALLOCATION_HPP

#include <variant>

#include "gpu/renderobject.hpp"
#include "gpu/texturebufferdesc.hpp"
#include "gpu/renderpipelinedesc.hpp"
#include "gpu/shaderbufferdesc.hpp"

#include "resource/uri.hpp"

namespace xng {
    /**
     * Describes an allocation operation by a frame graph pass.
     */
    struct FrameGraphAllocation {
        RenderObject::Type objectType; // The type of the object to allocate
        bool isUri; // If true the allocationData is used as a Uri to allocate the object, otherwise the corresponding desc type is used
        std::variant<Uri,
                std::pair<ResourceHandle<Shader>, RenderPipelineDesc>,
                TextureBufferDesc,
                ShaderBufferDesc,
                RenderTargetDesc> allocationData;
    };
}
#endif //XENGINE_FRAMEGRAPHALLOCATION_HPP
