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

#ifndef XENGINE_RENDERDEVICE_HPP
#define XENGINE_RENDERDEVICE_HPP

#include <map>
#include <functional>

#include "graphicsbackend.hpp"
#include "renderer.hpp"
#include "renderobject.hpp"
#include "renderallocator.hpp"

#include "asset/mesh.hpp"

namespace xengine {
    class XENGINE_EXPORT RenderDevice {
    public:
        static std::unique_ptr<RenderDevice> create(GraphicsBackend backend);

        virtual ~RenderDevice() = default;

        virtual Renderer &getRenderer() = 0;

        virtual RenderAllocator &getAllocator() = 0;

        /**
         * Returns the maximum number of samples supported in a multi sampled render target or texture.
         *
         * @return
         */
        virtual int getMaxSampleCount() = 0;

        virtual ShaderLanguage getPreferredShaderLanguage() = 0;

        virtual GraphicsBackend getBackend() = 0;
    };
}

#endif //XENGINE_RENDERDEVICE_HPP
