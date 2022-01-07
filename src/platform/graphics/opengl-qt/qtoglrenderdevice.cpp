/**
 *  XEngine - C++ game engine library
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

#include "qtoglrenderdevice.hpp"
#include "qtogltexturebuffer.hpp"
#include "qtoglcheckerror.hpp"
#include "qtoglmeshbuffer.hpp"
#include "qtoglshaderprogram.hpp"

namespace xengine {
    namespace opengl {
        Renderer &QtOGLRenderDevice::getRenderer() {
            return dynamic_cast<Renderer &>(renderer);
        }

        RenderAllocator &QtOGLRenderDevice::getAllocator() {
            return dynamic_cast<RenderAllocator &>(allocator);
        }

        int QtOGLRenderDevice::getMaxSampleCount() {
            return 0;
        }

        ShaderLanguage QtOGLRenderDevice::getPreferredShaderLanguage() {
            return GLSL_460;
        }

        GraphicsBackend QtOGLRenderDevice::getBackend() {
            return OPENGL_4_6_QT;
        }
    }
}