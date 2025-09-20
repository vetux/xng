/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_RENDERBATCH2D_HPP
#define XENGINE_RENDERBATCH2D_HPP

#include <vector>

#include "xng/graphics/camera.hpp"
#include "xng/graphics/image.hpp"
#include "xng/graphics/2d/drawcommand2d.hpp"
#include "xng/graphics/2d/texture2d.hpp"

namespace xng {
    struct RenderBatch2D {
        Camera camera;
        Transform cameraTransform;

        Mat4f viewProjectionMatrix;

        Vec2i mViewportOffset = {};
        Vec2i mViewportSize = Vec2i(1);

        bool mClear = false;

        ColorRGBA mClearColor = ColorRGBA::black();

        std::vector<DrawCommand2D> drawCommands;

        std::unordered_map<Texture2D::Handle, ImageRGBA> textureAllocations;
        std::vector<Texture2D::Handle> textureDeallocations;

        bool renderToScreen = true;
        Texture2D::Handle renderTarget{};

        Transform transform;
        bool worldSpace = false;
        Vec2i canvasSize;

        RenderBatch2D() = default;

        RenderBatch2D(const Camera &camera,
                      const Transform &camera_transform,
                      const Mat4f &view_projection_matrix,
                      const Vec2i &m_viewport_offset,
                      const Vec2i &m_viewport_size,
                      bool m_clear,
                      const ColorRGBA &m_clear_color,
                      const std::vector<DrawCommand2D> &draw_commands,
                      const std::unordered_map<Texture2D::Handle, ImageRGBA> &textureAllocations,
                      const std::vector<Texture2D::Handle> &textureDeallocations,
                      const bool renderToScreen,
                      const Texture2D::Handle renderTarget)
            : camera(camera),
              cameraTransform(camera_transform),
              viewProjectionMatrix(view_projection_matrix),
              mViewportOffset(m_viewport_offset),
              mViewportSize(m_viewport_size),
              mClear(m_clear),
              mClearColor(m_clear_color),
              drawCommands(draw_commands),
              textureAllocations(textureAllocations),
              textureDeallocations(textureDeallocations),
              renderToScreen(renderToScreen),
              renderTarget(renderTarget) {
        }
    };
}

#endif //XENGINE_RENDERBATCH2D_HPP
