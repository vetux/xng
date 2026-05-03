/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_RENDERGRAPH_PIPELINE_HPP
#define XENGINE_RENDERGRAPH_PIPELINE_HPP

#include <numeric>
#include <vector>
#include <optional>
#include <variant>

#include "xng/rendergraph/primitive.hpp"
#include "xng/rendergraph/shader/shader.hpp"
#include "xng/rendergraph/shader/shaderattributelayout.hpp"

namespace xng::rg {
    struct RasterPipeline {
        enum DepthTestMode {
            DEPTH_TEST_ALWAYS,
            DEPTH_TEST_NEVER,
            DEPTH_TEST_LESS,
            DEPTH_TEST_EQUAL,
            DEPTH_TEST_LEQUAL,
            DEPTH_TEST_GREATER,
            DEPTH_TEST_NOTEQUAL,
            DEPTH_TEST_GEQUAL
        };

        enum StencilMode {
            STENCIL_NEVER,
            STENCIL_LESS,
            STENCIL_LEQUAL,
            STENCIL_GREATER,
            STENCIL_GEQUAL,
            STENCIL_EQUAL,
            STENCIL_NOTEQUAL,
            STENCIL_ALWAYS
        };

        enum StencilAction {
            STENCIL_KEEP,
            STENCIL_ZERO,
            STENCIL_REPLACE,
            STENCIL_INCR,
            STENCIL_INCR_WRAP,
            STENCIL_DECR,
            STENCIL_DECR_WRAP,
            STENCIL_INVERT
        };

        enum FaceCullingMode {
            CULL_NONE,
            CULL_FRONT,
            CULL_BACK
        };

        enum FaceCullingWinding {
            CLOCKWISE,
            COUNTER_CLOCKWISE
        };

        enum BlendMode {
            ZERO,
            ONE,
            SRC_COLOR,
            ONE_MINUS_SRC_COLOR,
            DST_COLOR,
            SRC_ALPHA,
            ONE_MINUS_SRC_ALPHA,
            DST_ALPHA,
            ONE_MINUS_DST_ALPHA,
            CONSTANT_COLOR,
            ONE_MINUS_CONSTANT_COLOR,
            CONSTANT_ALPHA,
            ONE_MINUS_CONSTANT_ALPHA
        };

        enum BlendEquation {
            BLEND_ADD,
            BLEND_SUBTRACT,
            BLEND_REVERSE_SUBTRACT,
            BLEND_MIN,
            BLEND_MAX
        };

        struct VertexFormat {
            ShaderAttributeLayout layout;
            std::vector<size_t> bindingPoints; // The bindings points of each attribute
            std::vector<size_t> offsets; // The relative offsets of the attributes

            /**
             * @param layout The layout of the vertex
             * @param offsets The relative offsets of each attribute
             */
            VertexFormat(ShaderAttributeLayout layout, std::vector<size_t> bindingPoints, std::vector<size_t> offsets)
                : layout(std::move(layout)), bindingPoints(std::move(bindingPoints)), offsets(std::move(offsets)) {
                if (this->layout.getElements().size() != this->offsets.size()) {
                    throw std::runtime_error("Vertex layout and offsets must have the same number of elements");
                }
            }

            /**
             * Initializes the offsets to assume tightly packed attributes in a single buffer.
             *
             * @param layout The layout of the vertex
             */
            explicit VertexFormat(ShaderAttributeLayout layout)
                : layout(std::move(layout)) {
                size_t offset = 0;
                for (auto &element : this->layout.getElements()) {
                    offsets.emplace_back(offset);
                    bindingPoints.emplace_back(0);
                    offset += element.getSize();
                }
            }
        };

        std::vector<Shader> shaders;

        std::vector<ColorFormat> colorAttachments;
        std::optional<ColorFormat> depthAttachment;// Specifies the format of the texture bound as the depth attachment.
        std::optional<ColorFormat> stencilAttachment;// Specifies the format of the texture bound as the stencil attachment.

        Primitive primitive = TRIANGLES;
        bool multisample = false;
        bool multiSampleEnableFrequency = false;
        float multiSampleFrequency = 1;

        bool enableDepthTest = false;
        bool depthTestWrite = false;

        DepthTestMode depthTestMode = DEPTH_TEST_LESS;

        bool enableStencilTest = false;
        unsigned int stencilTestMask = 0xFF;
        StencilMode stencilMode = STENCIL_ALWAYS;
        int stencilReference = 1;
        unsigned int stencilFunctionMask = 0xFF;
        StencilAction stencilFail = STENCIL_KEEP;
        StencilAction stencilDepthFail = STENCIL_KEEP;
        StencilAction stencilPass = STENCIL_KEEP;

        bool enableFaceCulling = false;
        FaceCullingMode faceCullMode = CULL_BACK;
        FaceCullingWinding faceCullWinding = COUNTER_CLOCKWISE;

        bool enableBlending = false;
        BlendMode colorBlendSourceMode = SRC_ALPHA;
        BlendMode colorBlendDestinationMode = ONE_MINUS_SRC_ALPHA;
        BlendMode alphaBlendSourceMode = SRC_ALPHA;
        BlendMode alphaBlendDestinationMode = ONE_MINUS_SRC_ALPHA;
        BlendEquation colorBlendEquation = BLEND_ADD;
        BlendEquation alphaBlendEquation = BLEND_ADD;

        VertexFormat vertexFormat;
    };
}

#endif //XENGINE_RENDERGRAPH_PIPELINE_HPP
