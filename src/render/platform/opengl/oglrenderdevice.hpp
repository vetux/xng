/**
 *  xEngine - C++ game engine library
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

#ifndef XENGINE_OGLRENDERDEVICE_HPP
#define XENGINE_OGLRENDERDEVICE_HPP

#include "render/platform/renderdevice.hpp"

#include "render/platform/opengl/oglrenderer.hpp"
#include "render/platform/opengl/oglrenderallocator.hpp"

namespace xengine {
    namespace opengl {
        class OGLRenderDevice : public RenderDevice {
        public:
            OGLRenderDevice() = default;

            ~OGLRenderDevice() override = default;

            Renderer &getRenderer() override;

            RenderAllocator &getAllocator() override;

            int getMaxSampleCount() override;

            GraphicsBackend getBackend() override;

        private:
            OGLRenderer renderer;
            OGLRenderAllocator allocator;
        };
    }
}

#endif //XENGINE_OGLRENDERDEVICE_HPP
