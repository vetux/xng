/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2026 Julia Zampiccoli
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

#ifndef XENGINE_RENDERCANVAS_HPP
#define XENGINE_RENDERCANVAS_HPP

#include "xng/renderer/objects/rendermesh.hpp"
#include "xng/renderer/objects/renderpaint.hpp"
#include "xng/renderer/renderobject.hpp"

namespace xng {
    class RenderCanvas final : public RenderObject {
    public:
        RenderCanvas(const Id id,
                     BufferStreamer<ShaderTransform::CPU> &transformStream,
                     RenderObjectHandle<RenderMesh> _mesh,
                     std::vector<RenderObjectHandle<RenderPaint> > _paint)
            : RenderObject(id, OBJECT_CANVAS), transformStream(transformStream), mesh(std::move(_mesh)), paint(std::move(_paint)) {
        }

        ~RenderCanvas() = default;

    private:
        BufferStreamer<ShaderTransform::CPU> &transformStream;
        BufferStreamer<ShaderTransform::CPU>::Handle transformHandle;

        RenderObjectHandle<RenderMesh> mesh;
        std::vector<RenderObjectHandle<RenderPaint> > paint;
    };
}

#endif //XENGINE_RENDERCANVAS_HPP
