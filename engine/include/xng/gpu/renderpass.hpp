/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_RENDERPASS_HPP
#define XENGINE_RENDERPASS_HPP

#include <utility>

#include "xng/gpu/renderobject.hpp"
#include "xng/gpu/renderpassdesc.hpp"
#include "xng/gpu/vertexbuffer.hpp"
#include "xng/gpu/rendertarget.hpp"
#include "xng/gpu/commandfence.hpp"
#include "xng/gpu/vertexarrayobject.hpp"
#include "xng/gpu/texturearraybuffer.hpp"
#include "xng/gpu/drawcall.hpp"

namespace xng {
    class RenderPass : public RenderObject {
    public:
        Type getType() override {
            return RENDER_OBJECT_RENDER_PASS;
        }

        virtual const RenderPassDesc &getDescription() = 0;

        Command begin(RenderTarget &target) {
            return {Command::BEGIN_PASS, RenderPassBegin(this, &target)};
        }

        Command end() {
            return {Command::END_PASS, {}};
        }

        Command setViewport(Vec2i viewportOffset, Vec2i viewportSize) {
            return {Command::SET_VIEWPORT,
                    RenderPassViewport(std::move(viewportOffset), std::move(viewportSize))};
        }

        Command clearColorAttachments(ColorRGBA clearColor) {
            return {Command::CLEAR_COLOR, RenderPassClear(clearColor, {})};
        }

        Command clearDepthAttachment(float clearDepthValue) {
            return {Command::CLEAR_DEPTH, RenderPassClear({}, clearDepthValue)};
        }

        /**
         * Draw without indexing.
         *
         * @param drawCall
         */
        Command drawArray(const DrawCall &drawCall) {
            return {Command::DRAW_ARRAY, RenderPassDraw({drawCall}, {}, {})};
        }

        /**
         * Draw with indexing.
         *
         * @param drawCall
         */
        Command drawIndexed(const DrawCall &drawCall) {
            return {Command::DRAW_INDEXED, RenderPassDraw({drawCall}, {}, {})};
        }

        /**
         * Draw using instancing.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_InstanceID can be used in shaders to access the current instance index
         *
         * @param drawCall
         * @param numberOfInstances
         */
        Command instancedDrawArray(const DrawCall &drawCall, size_t numberOfInstances) {
            return {Command::DRAW_ARRAY_INSTANCED, RenderPassDraw({drawCall}, numberOfInstances, {})};
        }

        /**
         * Draw using instancing.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_InstanceID can be used in shaders to access the current instance index
         *
         * @param drawCall
         * @param numberOfInstances
         */
        Command instancedDrawIndexed(const DrawCall &drawCall, size_t numberOfInstances) {
            return {Command::DRAW_INDEXED_INSTANCED, RenderPassDraw({drawCall}, numberOfInstances, {})};
        }

        /**
         * Draw multiple commands with one draw call.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_MULTI_DRAW
         *
         * gl_DrawID can be used in shaders to access the current command index
         *
         * @param drawCalls
         */
        Command multiDrawArray(const std::vector<DrawCall> &drawCalls) {
            return {Command::DRAW_ARRAY_MULTI, RenderPassDraw(drawCalls, {}, {})};
        }

        /**
         * Draw multiple commands with one draw call.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_MULTI_DRAW
         *
         * gl_DrawID can be used in shaders to access the current command index
         *
         * @param drawCalls
         */
        Command multiDrawIndexed(const std::vector<DrawCall> &drawCalls) {
            return {Command::DRAW_INDEXED_MULTI, RenderPassDraw(drawCalls, {}, {})};
        }

        /**
         * Draw with indexing and optional offset to apply when indexing into the vertex buffer.
         *
         * The baseVertex is an offset that is applied to each index read from the index buffer before indexing the vertex buffer.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_BASE_VERTEX
         *
         * @param drawCall
         * @param baseVertex
         */
        Command drawIndexed(const DrawCall &drawCall, size_t baseVertex) {
            return {Command::DRAW_INDEXED_BASE_VERTEX, RenderPassDraw({drawCall}, {}, {baseVertex})};
        }

        /**
         * Draw using instancing and optional offset to apply when indexing into the vertex buffer.
         *
         * The baseVertex is an offset that is applied to each index read from the index buffer before indexing the vertex buffer.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_BASE_VERTEX and RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_InstanceID can be used in shaders to access the current instance index
         *
         * @param drawCall
         * @param numberOfInstances
         * @param baseVertex
         */
        Command instancedDrawIndexed(const DrawCall &drawCall, size_t numberOfInstances, size_t baseVertex) {
            return {Command::DRAW_INDEXED_INSTANCED_BASE_VERTEX,
                    RenderPassDraw({drawCall}, numberOfInstances, {baseVertex})};
        }

        /**
         * Draw multiple commands with one draw call and optional offset to apply when indexing into the vertex buffer.
         *
         * The baseVertex is an offset that is applied to each index read from the index buffer before indexing the vertex buffer.
         *
         * Requires RenderDeviceCapability.RENDER_PIPELINE_BASE_VERTEX and RenderDeviceCapability.RENDER_PIPELINE_INSTANCING
         *
         * gl_DrawID can be used in shaders to access the current command index
         *
         * @param drawCalls
         * @param baseVertices
         */
        Command multiDrawIndexed(const std::vector<DrawCall> &drawCalls, std::vector<size_t> baseVertices) {
            return {Command::DRAW_INDEXED_MULTI_BASE_VERTEX,
                    RenderPassDraw(drawCalls, {}, std::move(baseVertices))};
        }

        static Command debugBeginGroup(const std::string &name) {
            return {Command::DEBUG_BEGIN_GROUP, DebugGroup(name)};
        }

        static Command debugEndGroup() {
            return {Command::DEBUG_END_GROUP, {}};
        }
    };
}

#endif //XENGINE_RENDERPASS_HPP
