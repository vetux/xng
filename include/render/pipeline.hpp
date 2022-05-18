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

#ifndef XENGINE_PIPELINE_HPP
#define XENGINE_PIPELINE_HPP

#include "asset/scene.hpp"
#include "render/platform/rendertarget.hpp"

namespace xengine {
    class XENGINE_EXPORT Pipeline {
    public:
        virtual void render(RenderTarget &target, Scene &scene) = 0;

        virtual void setRenderResolution(Vec2i res) = 0;

        virtual void setRenderSamples(int samples) = 0;
    };
}
#endif //XENGINE_PIPELINE_HPP
