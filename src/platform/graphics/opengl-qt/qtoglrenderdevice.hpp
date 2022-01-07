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

#ifndef XENGINE_QTOGLRENDERDEVICE_HPP
#define XENGINE_QTOGLRENDERDEVICE_HPP

#include "platform/graphics/renderdevice.hpp"

#include "qtoglrenderer.hpp"
#include "qtoglrenderallocator.hpp"

#include <QOpenGLFunctions_4_5_Core>

namespace xengine {
    namespace opengl {
        class QtOGLRenderDevice : public RenderDevice, public QOpenGLFunctions_4_5_Core {
        public:
            QtOGLRenderDevice() {
                QOpenGLFunctions_4_5_Core::initializeOpenGLFunctions();
            }

            ~QtOGLRenderDevice() override = default;

            Renderer &getRenderer() override;

            RenderAllocator &getAllocator() override;

            int getMaxSampleCount() override;

            ShaderLanguage getPreferredShaderLanguage() override;

            GraphicsBackend getBackend() override;

        private:
            QtOGLRenderer renderer;
            QtOGLRenderAllocator allocator;
        };
    }
}

#endif //XENGINE_QTOGLRENDERDEVICE_HPP
