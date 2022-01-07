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

#ifndef MANA_RENDERER_HPP
#define MANA_RENDERER_HPP

#include "asset/image.hpp"
#include "rendercommand.hpp"
#include "rendertarget.hpp"
#include "meshbuffer.hpp"
#include "texturebuffer.hpp"
#include "graphicsbackend.hpp"

namespace engine {
    struct MANA_EXPORT RenderOptions {
        explicit RenderOptions(Vec2i viewportOffset,
                               Vec2i viewportSize,
                               bool multiSample = true,
                               ColorRGBA clearColorValue = {0, 0, 0, 0},
                               float clearDepthValue = 1,
                               bool clearColor = true,
                               bool clearDepth = true,
                               bool clearStencil = true)
                : viewportOffset(viewportOffset), viewportSize(viewportSize),
                  multiSample(multiSample),
                  clearColorValue(clearColorValue),
                  clearDepthValue(clearDepthValue),
                  clearColor(clearColor),
                  clearDepth(clearDepth),
                  clearStencil(clearStencil) {}

        Vec2i viewportOffset;
        Vec2i viewportSize;
        bool multiSample;
        ColorRGBA clearColorValue;
        float clearDepthValue;
        bool clearColor;
        bool clearDepth;
        bool clearStencil;
    };

    class MANA_EXPORT Renderer {
    public:
        virtual ~Renderer() = default;

        virtual void renderBegin(RenderTarget &target, const RenderOptions &options) = 0;

        virtual void addCommand(RenderCommand &command) = 0;

        virtual void renderFinish() = 0;

        virtual void renderClear(RenderTarget &target, ColorRGBA color) = 0;

        virtual void debugDrawCallRecordStart() = 0;

        virtual unsigned long debugDrawCallRecordStop() = 0;
    };
}

#endif //MANA_RENDERER_HPP
