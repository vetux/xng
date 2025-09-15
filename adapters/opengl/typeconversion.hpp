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

#ifndef XENGINE_TYPECONVERSION_HPP
#define XENGINE_TYPECONVERSION_HPP

#include "glad/glad.h"

#include "xng/rendergraph/rendergraphtextureproperties.hpp"
#include "xng/rendergraph/rendergraphpipeline.hpp"
#include "xng/rendergraph/drawcall.hpp"

using namespace xng;

static GLenum getColorAttachment(int index) {
    return GL_COLOR_ATTACHMENT0 + index;
}

static GLuint getTextureSlot(size_t slot) {
    return GL_TEXTURE0 + slot;
}

static GLenum getType(const ShaderDataType::Component c) {
    switch (c) {
        case ShaderDataType::BOOLEAN:
            return GL_BYTE;
        case ShaderDataType::UNSIGNED_INT:
            return GL_UNSIGNED_INT;
        case ShaderDataType::SIGNED_INT:
            return GL_INT;
        case ShaderDataType::FLOAT:
            return GL_FLOAT;
        case ShaderDataType::DOUBLE:
            return GL_DOUBLE;
        default:
            throw std::runtime_error("Invalid component");
    }
}

static GLenum convert(RenderGraphPipeline::FaceCullingMode mode) {
    switch (mode) {
        case RenderGraphPipeline::CULL_NONE:
            return GL_NONE;
        case RenderGraphPipeline::CULL_FRONT:
            return GL_FRONT;
        case RenderGraphPipeline::CULL_BACK:
            return GL_BACK;
    }
    throw std::runtime_error("Unsupported culling mode");
}

static GLenum convert(IndexFormat type) {
    switch (type) {
        case UNSIGNED_INT:
            return GL_UNSIGNED_INT;
    }
    throw std::runtime_error("Unsupported index type");
}

static GLenum convert(Primitive prim) {
    switch (prim) {
        case POINTS:
            return GL_POINTS;
        case LINES:
            return GL_LINES;
        case TRIANGLES:
            return GL_TRIANGLES;
        case QUAD:
            return GL_QUADS;
        default:
            throw std::runtime_error("Invalid primitive");
    }
}

static GLenum convert(const RenderGraphPipeline::DepthTestMode &mode) {
    switch (mode) {
        case RenderGraphPipeline::DEPTH_TEST_ALWAYS:
            return GL_ALWAYS;
        case RenderGraphPipeline::DEPTH_TEST_NEVER:
            return GL_NEVER;
        case RenderGraphPipeline::DEPTH_TEST_LESS:
            return GL_LESS;
        case RenderGraphPipeline::DEPTH_TEST_EQUAL:
            return GL_EQUAL;
        case RenderGraphPipeline::DEPTH_TEST_LEQUAL:
            return GL_LEQUAL;
        case RenderGraphPipeline::DEPTH_TEST_GREATER:
            return GL_GREATER;
        case RenderGraphPipeline::DEPTH_TEST_NOTEQUAL:
            return GL_NOTEQUAL;
        case RenderGraphPipeline::DEPTH_TEST_GEQUAL:
            return GL_GEQUAL;
    }
    throw std::runtime_error("Unsupported depth testing mode");
}

static GLenum convert(RenderGraphPipeline::BlendMode mode) {
    switch (mode) {
        case RenderGraphPipeline::ZERO:
            return GL_ZERO;
        case RenderGraphPipeline::ONE:
            return GL_ONE;
        case RenderGraphPipeline::SRC_COLOR:
            return GL_SRC_COLOR;
        case RenderGraphPipeline::ONE_MINUS_SRC_COLOR:
            return GL_ONE_MINUS_SRC_COLOR;
        case RenderGraphPipeline::DST_COLOR:
            return GL_DST_COLOR;
        case RenderGraphPipeline::SRC_ALPHA:
            return GL_SRC_ALPHA;
        case RenderGraphPipeline::ONE_MINUS_SRC_ALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
        case RenderGraphPipeline::DST_ALPHA:
            return GL_DST_ALPHA;
        case RenderGraphPipeline::ONE_MINUS_DST_ALPHA:
            return GL_ONE_MINUS_DST_ALPHA;
        case RenderGraphPipeline::CONSTANT_COLOR:
            return GL_CONSTANT_COLOR;
        case RenderGraphPipeline::ONE_MINUS_CONSTANT_COLOR:
            return GL_ONE_MINUS_CONSTANT_COLOR;
        case RenderGraphPipeline::CONSTANT_ALPHA:
            return GL_CONSTANT_ALPHA;
        case RenderGraphPipeline::ONE_MINUS_CONSTANT_ALPHA:
            return GL_ONE_MINUS_CONSTANT_ALPHA;
    }
    throw std::runtime_error("Unsupported blending mode");
}

static GLenum convert(RenderGraphPipeline::BlendEquation eq) {
    switch (eq) {
        case RenderGraphPipeline::BLEND_ADD:
            return GL_FUNC_ADD;
        case RenderGraphPipeline::BLEND_SUBTRACT:
            return GL_FUNC_SUBTRACT;
        case RenderGraphPipeline::BLEND_REVERSE_SUBTRACT:
            return GL_FUNC_REVERSE_SUBTRACT;
        case RenderGraphPipeline::BLEND_MIN:
            return GL_MIN;
        case RenderGraphPipeline::BLEND_MAX:
            return GL_MAX;
    }
    throw std::runtime_error("Unsupported blend equation");
}

static GLenum convert(RenderGraphPipeline::StencilMode mode) {
    switch (mode) {
        case RenderGraphPipeline::STENCIL_NEVER:
            return GL_NEVER;
        case RenderGraphPipeline::STENCIL_LESS:
            return GL_LESS;
        case RenderGraphPipeline::STENCIL_LEQUAL:
            return GL_LEQUAL;
        case RenderGraphPipeline::STENCIL_GREATER:
            return GL_GREATER;
        case RenderGraphPipeline::STENCIL_GEQUAL:
            return GL_GEQUAL;
        case RenderGraphPipeline::STENCIL_EQUAL:
            return GL_EQUAL;
        case RenderGraphPipeline::STENCIL_NOTEQUAL:
            return GL_NOTEQUAL;
        case RenderGraphPipeline::STENCIL_ALWAYS:
            return GL_ALWAYS;
    }
    throw std::runtime_error("Unsupported stencil mode");
}

static GLenum convert(RenderGraphPipeline::StencilAction action) {
    switch (action) {
        case RenderGraphPipeline::STENCIL_KEEP:
            return GL_KEEP;
        case RenderGraphPipeline::STENCIL_ZERO:
            return GL_ZERO;
        case RenderGraphPipeline::STENCIL_REPLACE:
            return GL_REPLACE;
        case RenderGraphPipeline::STENCIL_INCR:
            return GL_INCR;
        case RenderGraphPipeline::STENCIL_INCR_WRAP:
            return GL_INCR_WRAP;
        case RenderGraphPipeline::STENCIL_DECR:
            return GL_DECR;
        case RenderGraphPipeline::STENCIL_DECR_WRAP:
            return GL_DECR_WRAP;
        case RenderGraphPipeline::STENCIL_INVERT:
            return GL_INVERT;
    }
    throw std::runtime_error("Unsupported stencil action");
}

static GLenum convert(TextureType type) {
    switch (type) {
        case TEXTURE_2D:
            return GL_TEXTURE_2D;
        case TEXTURE_2D_MULTISAMPLE:
            return GL_TEXTURE_2D_MULTISAMPLE;
        case TEXTURE_CUBE_MAP:
            return GL_TEXTURE_CUBE_MAP;
        default:
            throw std::runtime_error("Unrecognized texture type");
    }
}

static GLenum convert(CubeMapFace face) {
    switch (face) {
        case POSITIVE_X:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        case NEGATIVE_X:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case POSITIVE_Y:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        case NEGATIVE_Y:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case POSITIVE_Z:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        case NEGATIVE_Z:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        default:
            throw std::runtime_error("Unrecognized cube map face");
    }
}

static GLint convert(ColorFormat format) {
    switch (format) {
        case DEPTH:
            return GL_DEPTH_COMPONENT;
        case DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        case R:
            return GL_RED;
        case RG:
            return GL_RG;
        case RGB:
            return GL_RGB;
        case RGBA:
            return GL_RGBA;
        case R_COMPRESSED:
            return GL_COMPRESSED_RED;
        case RG_COMPRESSED:
            return GL_COMPRESSED_RG;
        case RGB_COMPRESSED:
            return GL_COMPRESSED_RGB;
        case RGBA_COMPRESSED:
            return GL_COMPRESSED_RGBA;
        case R8:
            return GL_R8;
        case RG8:
            return GL_RG8;
        case RGB8:
            return GL_RGB8;
        case RGBA8:
            return GL_RGBA8;
        case R16:
            return GL_R16;
        case RG16:
            return GL_RG16;
        case RGB16:
            return GL_RGB16;
        case RGBA16:
            return GL_RGBA16;
        case RGB12:
            return GL_RGB12;
        case RGBA12:
            return GL_RGBA12;
        case RGB10:
            return GL_RGB10;
        case R16F:
            return GL_R16F;
        case RG16F:
            return GL_RG16F;
        case RGB16F:
            return GL_RGB16F;
        case RGBA16F:
            return GL_RGBA16F;
        case R32F:
            return GL_R32F;
        case RG32F:
            return GL_RG32F;
        case RGB32F:
            return GL_RGB32F;
        case RGBA32F:
            return GL_RGBA32F;
        case R8I:
            return GL_R8I;
        case RG8I:
            return GL_RG8I;
        case RGB8I:
            return GL_RGB8I;
        case RGBA8I:
            return GL_RGBA8I;
        case R16I:
            return GL_R16I;
        case RG16I:
            return GL_RG16I;
        case RGB16I:
            return GL_RGB16I;
        case RGBA16I:
            return GL_RGBA16I;
        case R32I:
            return GL_R32I;
        case RG32I:
            return GL_RG32I;
        case RGB32I:
            return GL_RGB32I;
        case RGBA32I:
            return GL_RGBA32I;
        case R8UI:
            return GL_R8UI;
        case RG8UI:
            return GL_RG8UI;
        case RGB8UI:
            return GL_RGB8UI;
        case RGBA8UI:
            return GL_RGBA8UI;
        case R16UI:
            return GL_R16UI;
        case RG16UI:
            return GL_RG16UI;
        case RGB16UI:
            return GL_RGB16UI;
        case RGBA16UI:
            return GL_RGBA16UI;
        case R32UI:
            return GL_R32UI;
        case RG32UI:
            return GL_RG32UI;
        case RGB32UI:
            return GL_RGB32UI;
        case RGBA32UI:
            return GL_RGBA32UI;
        default:
            throw std::runtime_error("Unrecognized color format");
    }
}

static GLint convert(TextureWrapping wrapping) {
    switch (wrapping) {
        case REPEAT:
            return GL_REPEAT;
        case MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
    }
    throw std::runtime_error("Invalid texture wrapping");
}

static GLint convert(TextureFiltering filtering) {
    switch (filtering) {
        case NEAREST:
            return GL_NEAREST;
        case LINEAR:
            return GL_LINEAR;
    }
    throw std::runtime_error("Invalid texture filtering");
}

static GLint convert(MipMapFiltering filtering) {
    switch (filtering) {
        case NEAREST_MIPMAP_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        case LINEAR_MIPMAP_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;
        case NEAREST_MIPMAP_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        case LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
    }
    throw std::runtime_error("Invalid mipmap filtering");
}

#endif //XENGINE_TYPECONVERSION_HPP
