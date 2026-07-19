/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_RENDERMATERIAL_HPP
#define XENGINE_RENDERMATERIAL_HPP

#include "xng/renderer/shadingmodel.hpp"

#include "xng/renderer/objects/rendertexture.hpp"
#include "xng/renderer/pipeline/renderpipeline.hpp"

namespace xng {
    class RenderMaterial {
    public:
        explicit RenderMaterial(std::shared_ptr<RenderPipelineMaterial> materialHandle)
            : materialHandle(std::move(materialHandle)) {
        }

        RenderPipelineMaterial &getHandle() const {
            return *materialHandle;
        }

    private:
        std::shared_ptr<RenderPipelineMaterial> materialHandle;
    };
}

#endif //XENGINE_RENDERMATERIAL_HPP
