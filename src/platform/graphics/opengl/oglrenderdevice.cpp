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

#ifdef BUILD_ENGINE_RENDERER_OPENGL

#include "oglrenderdevice.hpp"
#include "ogltexturebuffer.hpp"
#include "oglcheckerror.hpp"
#include "oglmeshbuffer.hpp"
#include "oglshaderprogram.hpp"

namespace engine {
    namespace opengl {
        Renderer &OGLRenderDevice::getRenderer() {
            return dynamic_cast<Renderer &>(renderer);
        }

        RenderAllocator &OGLRenderDevice::getAllocator() {
            return dynamic_cast<RenderAllocator &>(allocator);
        }

        int OGLRenderDevice::getMaxSampleCount() {
            int ret = 0;
            glGetIntegerv(GL_MAX_SAMPLES, &ret);
            return ret;
        }

        ShaderLanguage OGLRenderDevice::getPreferredShaderLanguage() {
            return GLSL_460;
        }

        GraphicsBackend OGLRenderDevice::getBackend() {
            return OPENGL_4_6;
        }
    }
}

#endif