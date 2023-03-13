/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/asset/camera.hpp"
#include "xng/render/2d/renderer2d.hpp"

#include <utility>

#include "xng/math/matrixmath.hpp"
#include "xng/asset/shadersource.hpp"

#include "xng/geometry/vertexstream.hpp"

static const char *SHADER_VERT_TEXTURE = R"###(#version 320 es

layout (location = 0) in highp vec2 position;
layout (location = 1) in highp vec2 uv;

layout (location = 0) out highp vec4 fPosition;
layout (location = 1) out highp vec2 fUv;

struct PassData {
    highp vec4 color;
    highp vec4 colorMixFactor_alphaMixFactor_colorFactor;
    ivec4 texAtlasLevel_texAtlasIndex;
    highp mat4 mvp;
    highp vec4 uvOffset_uvScale;
    highp vec4 atlasScale;
};

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    PassData passData;
} vars;

layout(binding = 1) uniform highp sampler2DArray atlasTextures[12];

void main() {
    fPosition = (vars.passData.mvp) * vec4(position, 0, 1);
    fUv = uv;
    gl_Position = fPosition;
}
)###";

static const char *SHADER_FRAG_TEXTURE = R"###(#version 320 es

layout (location = 0) in highp vec4 fPosition;
layout (location = 1) in highp vec2 fUv;

layout (location = 0) out highp vec4 color;

struct PassData {
    highp vec4 color;
    highp vec4 colorMixFactor_alphaMixFactor_colorFactor;
    ivec4 texAtlasLevel_texAtlasIndex;
    highp mat4 mvp;
    highp vec4 uvOffset_uvScale;
    highp vec4 atlasScale;
};

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    PassData passData;
} vars;

layout(binding = 1) uniform highp sampler2DArray atlasTextures[12];

void main() {
    if (vars.passData.texAtlasLevel_texAtlasIndex.y >= 0) {
        highp vec2 uv = fUv;
        uv = uv * vars.passData.uvOffset_uvScale.zw;
        uv = uv + vars.passData.uvOffset_uvScale.xy;
        uv = uv * vars.passData.atlasScale.xy;
        highp vec4 texColor = texture(atlasTextures[vars.passData.texAtlasLevel_texAtlasIndex.x],
                                        vec3(uv.x, uv.y, vars.passData.texAtlasLevel_texAtlasIndex.y));
        if (vars.passData.colorMixFactor_alphaMixFactor_colorFactor.z != 0.f) {
            color = vars.passData.color * texColor;
        } else {
            color.rgb = mix(texColor.rgb, vars.passData.color.rgb, vars.passData.colorMixFactor_alphaMixFactor_colorFactor.x);
            color.a = mix(texColor.a, vars.passData.color.a, vars.passData.colorMixFactor_alphaMixFactor_colorFactor.y);
        }
    } else {
        color = vars.passData.color;
    }
}
)###";

static const char *SHADER_VERT_TEXTURE_MULTIDRAW = R"###(#version 460

#define MAX_MULTI_DRAW_COUNT 1000 // Maximum amount of draws per multi draw

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec4 fPosition;
layout (location = 1) out vec2 fUv;
layout (location = 2) flat out uint drawID;

struct PassData {
    vec4 color;
    vec4 colorMixFactor_alphaMixFactor_colorFactor;
    ivec4 texAtlasLevel_texAtlasIndex;
    mat4 mvp;
    vec4 uvOffset_uvScale;
    vec4 atlasScale;
};

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    PassData passes[MAX_MULTI_DRAW_COUNT];
} vars;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

void main() {
    fPosition = (vars.passes[gl_DrawID].mvp) * vec4(position, 0, 1);
    fUv = uv;
    drawID = gl_DrawID;
    gl_Position = fPosition;
}
)###";

static const char *SHADER_FRAG_TEXTURE_MULTIDRAW = R"###(#version 460

#define MAX_MULTI_DRAW_COUNT 1000 // Maximum amount of draws per multi draw

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;
layout (location = 2) flat in uint drawID;

layout (location = 0) out vec4 color;

struct PassData {
    vec4 color;
    vec4 colorMixFactor_alphaMixFactor_colorFactor;
    ivec4 texAtlasLevel_texAtlasIndex;
    mat4 mvp;
    vec4 uvOffset_uvScale;
    vec4 atlasScale;
};

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    PassData passes[MAX_MULTI_DRAW_COUNT];
} vars;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

void main() {
    if (vars.passes[drawID].texAtlasLevel_texAtlasIndex.y >= 0) {
        vec2 uv = fUv;
        uv = uv * vars.passes[drawID].uvOffset_uvScale.zw;
        uv = uv + vars.passes[drawID].uvOffset_uvScale.xy;
        uv = uv * vars.passes[drawID].atlasScale.xy;
        vec4 texColor = texture(atlasTextures[vars.passes[drawID].texAtlasLevel_texAtlasIndex.x],
                        vec3(uv.x, uv.y, vars.passes[drawID].texAtlasLevel_texAtlasIndex.y));
        if (vars.passes[drawID].colorMixFactor_alphaMixFactor_colorFactor.z != 0) {
            color = vars.passes[drawID].color * texColor;
        } else {
            color.rgb = mix(texColor.rgb, vars.passes[drawID].color.rgb, vars.passes[drawID].colorMixFactor_alphaMixFactor_colorFactor.x);
            color.a = mix(texColor.a, vars.passes[drawID].color.a, vars.passes[drawID].colorMixFactor_alphaMixFactor_colorFactor.y);
        }
    } else {
        color = vars.passes[drawID].color;
    }
}
)###";

static float distance(float val1, float val2) {
    float abs = val1 - val2;
    if (abs < 0)
        return abs * -1;
    else
        return abs;
}

namespace xng {
    static const int MAX_MULTI_DRAW_COUNT = 1000;

#pragma pack(push, 1)
    // If anything other than 4 component vectors are used in uniform buffer data the memory layout becomes unpredictable.
    struct PassData {
        float color[4];
        float colorMixFactor_alphaMixFactor_colorFactor[4];
        int texAtlasLevel_texAtlasIndex[4]{-1, -1};
        Mat4f mvp;
        float uvOffset_uvScale[4];
        float atlasScale[4];
    };

    struct ShaderUniformBuffer {
        PassData passData;
    };

    struct ShaderUniformBufferMultiDraw {
        PassData passes[MAX_MULTI_DRAW_COUNT];
    };
#pragma pack(pop)

    static_assert(sizeof(PassData) % 16 == 0);
    static_assert(sizeof(ShaderUniformBufferMultiDraw) % 16 == 0);

    struct DrawBatch {
        std::vector<RenderPass::DrawCall> drawCalls;
        std::vector<size_t> baseVertices;
        Primitive primitive = TRIANGLES;
        std::vector<ShaderUniformBuffer> uniformBuffers; // The uniform buffer data for this batch
    };

    /**
     * A draw cycle consists of one or more draw batches.
     *
     * Each draw batch contains draw calls which are dispatched using multiDraw.
     *
     * There is only MAX_MULTI_DRAW_COUNT draw calls per draw cycle.
     *
     * To ensure correct blending consequent draw calls of the same primitive are drawn in the same batch
     * together but each change in primitive causes a new draw batch to be created which incurs the overhead of
     * updating the shader uniform buffer.
     */
    struct DrawBatchMultiDraw {
        std::vector<RenderPass::DrawCall> drawCalls;
        std::vector<size_t> baseVertices;
        Primitive primitive = TRIANGLES;
        ShaderUniformBufferMultiDraw uniformBuffer; // The uniform buffer data for this batch
    };

    Renderer2D::Renderer2D(RenderDevice &device, ShaderCompiler &shaderCompiler, ShaderDecompiler &shaderDecompiler)
            : renderDevice(device) {
        vertexLayout.emplace_back(VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT));
        vertexLayout.emplace_back(VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT));

        vertexSize = 0;
        for (auto &attr: vertexLayout) {
            vertexSize += attr.stride();
        }

        VertexBufferDesc vertexBufferDesc;
        vertexBufferDesc.size = 0;
        vertexBuffer = renderDevice.createVertexBuffer(vertexBufferDesc);

        IndexBufferDesc indexBufferDesc;
        indexBufferDesc.size = 0;
        indexBuffer = renderDevice.createIndexBuffer(indexBufferDesc);

        vaoChange = true;

        VertexArrayObjectDesc vertexArrayObjectDesc;
        vertexArrayObjectDesc.vertexLayout = vertexLayout;
        vertexArrayObject = renderDevice.createVertexArrayObject(vertexArrayObjectDesc);

        vsTexture = ShaderSource(SHADER_VERT_TEXTURE, "main", VERTEX, GLSL_ES_320, false);
        fsTexture = ShaderSource(SHADER_FRAG_TEXTURE, "main", FRAGMENT, GLSL_ES_320, false);

        vsTexture = vsTexture.preprocess(shaderCompiler);
        fsTexture = fsTexture.preprocess(shaderCompiler);

        auto vsTexSource = vsTexture.compile(shaderCompiler);
        auto fsTexSource = fsTexture.compile(shaderCompiler);

        vsTextureMultiDraw = ShaderSource(SHADER_VERT_TEXTURE_MULTIDRAW, "main", VERTEX, GLSL_460, false);
        fsTextureMultiDraw = ShaderSource(SHADER_FRAG_TEXTURE_MULTIDRAW, "main", FRAGMENT, GLSL_460, false);

        vsTextureMultiDraw = vsTextureMultiDraw.preprocess(shaderCompiler);
        fsTextureMultiDraw = fsTextureMultiDraw.preprocess(shaderCompiler);

        auto vsTexSourceMultiDraw = vsTextureMultiDraw.compile(shaderCompiler);
        auto fsTexSourceMultiDraw = fsTextureMultiDraw.compile(shaderCompiler);

        RenderPipelineDesc desc;

        desc.shaders = {
                {ShaderStage::VERTEX,   vsTexSource.getShader()},
                {ShaderStage::FRAGMENT, fsTexSource.getShader()}
        };
        desc.bindings = {
                RenderPipelineBindingType::BIND_SHADER_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
                RenderPipelineBindingType::BIND_TEXTURE_ARRAY_BUFFER,
        };

        desc.vertexLayout = vertexLayout;
        desc.enableBlending = true;

        desc.primitive = TRIANGLES;
        trianglePipeline = device.createRenderPipeline(desc, shaderDecompiler);

        desc.primitive = LINES;
        linePipeline = device.createRenderPipeline(desc, shaderDecompiler);

        desc.primitive = POINTS;
        pointPipeline = device.createRenderPipeline(desc, shaderDecompiler);

        desc.shaders = {
                {ShaderStage::VERTEX,   vsTexSourceMultiDraw.getShader()},
                {ShaderStage::FRAGMENT, fsTexSourceMultiDraw.getShader()}
        };

        desc.primitive = TRIANGLES;
        trianglePipelineMultiDraw = device.createRenderPipeline(desc, shaderDecompiler);

        desc.primitive = LINES;
        linePipelineMultiDraw = device.createRenderPipeline(desc, shaderDecompiler);

        desc.primitive = POINTS;
        pointPipelineMultiDraw = device.createRenderPipeline(desc, shaderDecompiler);

        for (int i = TEXTURE_ATLAS_8x8; i < TEXTURE_ATLAS_END; i++) {
            auto res = (TextureAtlasResolution) i;
            TextureArrayBufferDesc atlasDesc;
            atlasDesc.textureDesc.size = TextureAtlas::getResolutionLevelSize(res);
            atlasDesc.textureDesc.generateMipmap = true;
            atlasTextures[res] = std::move(device.createTextureArrayBuffer(atlasDesc));
        }

        RenderPassDesc passDesc;
        passDesc.numberOfColorAttachments = 1;
        passDesc.hasDepthStencilAttachment = false;
        renderPass = device.createRenderPass(passDesc);

        rebindTextureAtlas({});
    }

    Renderer2D::~Renderer2D() = default;

    TextureAtlasHandle Renderer2D::createTexture(const ImageRGBA &texture) {
        auto res = TextureAtlas::getClosestMatchingResolutionLevel(texture.getSize());
        auto free = atlas.getFreeSlotCount(res);
        if (free < 1) {
            auto desc = atlasTextures.at(res)->getDescription();
            desc.textureCount += 1;
            auto buf = std::move(atlasTextures.at(res));
            atlasTextures.at(res) = renderDevice.createTextureArrayBuffer(desc);
            atlasTextures.at(res)->copy(*buf);
            auto occupations = atlas.getBufferOccupations();
            occupations[res].resize(desc.textureCount, false);
            rebindTextureAtlas(occupations);
        }
        return atlas.add(texture);
    }

    std::vector<TextureAtlasHandle> Renderer2D::createTextures(const std::vector<ImageRGBA> &textures) {
        std::map<TextureAtlasResolution, size_t> resolutionCounts;
        for (auto &img: textures) {
            auto res = TextureAtlas::getClosestMatchingResolutionLevel(img.getSize());
            resolutionCounts[res]++;
        }
        for (auto &pair: resolutionCounts) {
            auto free = atlas.getFreeSlotCount(pair.first);
            if (free < pair.second) {
                auto desc = atlasTextures.at(pair.first)->getDescription();
                desc.textureCount += pair.second - free;
                auto buffer = renderDevice.createTextureArrayBuffer(desc);
                buffer->copy(*atlasTextures.at(pair.first));
                atlasTextures.at(pair.first) = std::move(buffer);
                auto occupations = atlas.getBufferOccupations();
                occupations[pair.first].resize(desc.textureCount, false);
                rebindTextureAtlas(occupations);
            }
        }
        std::vector<TextureAtlasHandle> ret;
        ret.reserve(textures.size());
        for (auto &img: textures) {
            ret.emplace_back(atlas.add(img));
        }
        return ret;
    }

    void Renderer2D::destroyTexture(const TextureAtlasHandle &handle) {
        atlas.remove(handle);
    }

    void Renderer2D::renderBegin(RenderTarget &target,
                                 bool clear,
                                 const ColorRGBA &clearColor,
                                 const Vec2i &viewportOffset,
                                 const Vec2i &viewportSize,
                                 const Vec2f &cameraPosition,
                                 const Rectf &projection) {
        if (isRendering) {
            throw std::runtime_error("Already Rendering (Nested Renderer2D::renderBegin calls?)");
        }
        isRendering = true;
        mViewportOffset = viewportOffset;
        mViewportSize = viewportSize;

        auto prevCam = camera;

        camera.type = ORTHOGRAPHIC;
        camera.left = projection.position.x;
        camera.right = projection.dimensions.x;
        camera.top = projection.position.y;
        camera.bottom = projection.dimensions.y;

        auto prevTransform = cameraTransform;
        cameraTransform.setPosition({cameraPosition.x, cameraPosition.y, 1});

        if (camera != prevCam ||
            cameraPosition != Vec2f{cameraTransform.getPosition().x, cameraTransform.getPosition().y}) {
            viewProjectionMatrix = camera.projection() * Camera::view(cameraTransform);
        }

        if (clear) {
            renderPass->beginRenderPass(target, viewportOffset, viewportSize);
            renderPass->clearColorAttachments(clearColor);
            renderPass->clearDepthAttachments(1);
            renderPass->endRenderPass();
        }

        userTarget = &target;

        polyCounter = 0;
    }

    void Renderer2D::renderPresent() {
        if (!isRendering) {
            throw std::runtime_error("Not Rendering");
        }
        isRendering = false;

        auto caps = renderDevice.getInfo().capabilities;

        if (caps.find(CAPABILITY_MULTI_DRAW) != caps.end()) {
            presentMultiDraw();
        } else {
            if (caps.find(CAPABILITY_BASE_VERTEX) == caps.end()) {
                throw std::runtime_error("CAPABILITY_BASE_VERTEX is required");
            }
            presentCompat();
        }

        passes.clear();
    }

    void Renderer2D::draw(const Rectf &srcRect,
                          const Rectf &dstRect,
                          TextureAtlasHandle &sprite,
                          const Vec2f &center,
                          float rotation,
                          float mix,
                          float mixAlpha,
                          ColorRGBA mixColor) {
        passes.emplace_back(Pass(srcRect,
                                 dstRect,
                                 sprite,
                                 center,
                                 rotation,
                                 mix,
                                 mixAlpha,
                                 mixColor));
    }

    void Renderer2D::draw(const Rectf &srcRect,
                          const Rectf &dstRect,
                          TextureAtlasHandle &sprite,
                          const Vec2f &center,
                          float rotation,
                          ColorRGBA colorFactor) {
        passes.emplace_back(Pass(srcRect,
                                 dstRect,
                                 sprite,
                                 center,
                                 rotation,
                                 colorFactor));
    }

    void Renderer2D::draw(const Rectf &rectangle, ColorRGBA color, bool fill, const Vec2f &center, float rotation) {
        passes.emplace_back(Pass(rectangle, color, fill, center, rotation));
    }

    void
    Renderer2D::draw(const Vec2f &start, const Vec2f &end, ColorRGBA color, const Vec2f &position, const Vec2f &center,
                     float rotation) {
        passes.emplace_back(Pass(start, end, color, position, center, rotation));
    }

    void Renderer2D::draw(const Vec2f &point,
                          ColorRGBA color,
                          const Vec2f &position,
                          const Vec2f &center,
                          float rotation) {
        passes.emplace_back(Pass(point, color, position, center, rotation));
    }

    void Renderer2D::rebindTextureAtlas(const std::map<TextureAtlasResolution, std::vector<bool>> &occupations) {
        std::map<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>> atlasRef;

        atlasRef.insert(
                std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(TEXTURE_ATLAS_8x8,
                                                                                                   *atlasTextures.at(
                                                                                                           TEXTURE_ATLAS_8x8).get()));
        atlasRef.insert(
                std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(TEXTURE_ATLAS_16x16,
                                                                                                   *atlasTextures.at(
                                                                                                           TEXTURE_ATLAS_16x16).get()));
        atlasRef.insert(
                std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(TEXTURE_ATLAS_32x32,
                                                                                                   *atlasTextures.at(
                                                                                                           TEXTURE_ATLAS_32x32).get()));
        atlasRef.insert(
                std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(TEXTURE_ATLAS_64x64,
                                                                                                   *atlasTextures.at(
                                                                                                           TEXTURE_ATLAS_64x64).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_128x128, *atlasTextures.at(TEXTURE_ATLAS_128x128).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_256x256, *atlasTextures.at(TEXTURE_ATLAS_256x256).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_512x512, *atlasTextures.at(TEXTURE_ATLAS_512x512).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_1024x1024, *atlasTextures.at(TEXTURE_ATLAS_1024x1024).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_2048x2048, *atlasTextures.at(TEXTURE_ATLAS_2048x2048).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_4096x4096, *atlasTextures.at(TEXTURE_ATLAS_4096x4096).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_8192x8192, *atlasTextures.at(TEXTURE_ATLAS_8192x8192).get()));
        atlasRef.insert(std::make_pair<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>>(
                TEXTURE_ATLAS_16384x16384, *atlasTextures.at(TEXTURE_ATLAS_16384x16384).get()));

        atlas = TextureAtlas(atlasRef, occupations);
    }

    void Renderer2D::presentCompat() {
        // Compatibility render path for OpenGL ES 3.2

        usedPlanes.clear();
        usedSquares.clear();
        usedLines.clear();
        usedPoints.clear();
        usedRotationMatrices.clear();

        std::vector<RenderPass::DrawCall> drawCalls;
        std::vector<size_t> baseVertices;
        std::vector<Primitive> primitives;
        std::vector<ShaderUniformBuffer> shaderBuffers;
        std::vector<PassData> passData;

        for (auto &pass: passes) {
            switch (pass.type) {
                case Pass::COLOR_POINT: {
                    usedPoints.insert(pass.dstRect.position);
                    auto point = getPoint(pass.dstRect.position);

                    primitives.emplace_back(point.primitive);
                    baseVertices.emplace_back(point.baseVertex);
                    drawCalls.emplace_back(point.drawCall);

                    polyCounter += 1;

                    auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                    auto model = MatrixMath::translate({
                                                               pass.srcRect.position.x,
                                                               pass.srcRect.position.y,
                                                               0})
                                 * rotMat;

                    ShaderUniformBuffer buffer;

                    buffer.passData.mvp = viewProjectionMatrix * model;
                    auto color = pass.color.divide();
                    buffer.passData.color[0] = color.x;
                    buffer.passData.color[1] = color.y;
                    buffer.passData.color[2] = color.z;
                    buffer.passData.color[3] = color.w;

                    shaderBuffers.emplace_back(buffer);

                    break;
                }
                case Pass::COLOR_LINE: {
                    usedLines.insert(std::make_pair(pass.dstRect.position, pass.dstRect.dimensions));

                    auto line = getLine(pass.dstRect.position, pass.dstRect.dimensions);

                    primitives.emplace_back(line.primitive);
                    baseVertices.emplace_back(line.baseVertex);
                    drawCalls.emplace_back(line.drawCall);

                    polyCounter += 1;

                    auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                    auto model = MatrixMath::translate({
                                                               pass.srcRect.position.x,
                                                               pass.srcRect.position.y,
                                                               0})
                                 * rotMat;

                    ShaderUniformBuffer buffer;

                    buffer.passData.mvp = viewProjectionMatrix * model;
                    auto color = pass.color.divide();
                    buffer.passData.color[0] = color.x;
                    buffer.passData.color[1] = color.y;
                    buffer.passData.color[2] = color.z;
                    buffer.passData.color[3] = color.w;

                    shaderBuffers.emplace_back(buffer);
                    break;
                }
                case Pass::COLOR_PLANE: {
                    if (pass.fill) {
                        usedPlanes.insert(pass.dstRect.dimensions);
                        auto plane = getPlane(pass.dstRect.dimensions);

                        primitives.emplace_back(plane.primitive);
                        baseVertices.emplace_back(plane.baseVertex);
                        drawCalls.emplace_back(plane.drawCall);

                        polyCounter += 2;
                    } else {
                        usedSquares.insert(pass.dstRect.dimensions);
                        auto square = getSquare(pass.dstRect.dimensions);

                        primitives.emplace_back(square.primitive);
                        baseVertices.emplace_back(square.baseVertex);
                        drawCalls.emplace_back(square.drawCall);

                        polyCounter += 4;
                    }

                    auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                    auto model = MatrixMath::translate({
                                                               pass.dstRect.position.x,
                                                               pass.dstRect.position.y,
                                                               0})
                                 * rotMat;

                    ShaderUniformBuffer buffer;

                    buffer.passData.mvp = viewProjectionMatrix * model;
                    auto color = pass.color.divide();
                    buffer.passData.color[0] = color.x;
                    buffer.passData.color[1] = color.y;
                    buffer.passData.color[2] = color.z;
                    buffer.passData.color[3] = color.w;

                    shaderBuffers.emplace_back(buffer);
                    break;
                }
                case Pass::TEXTURE: {
                    usedPlanes.insert(pass.dstRect.dimensions);
                    auto plane = getPlane(pass.dstRect.dimensions);

                    primitives.emplace_back(plane.primitive);
                    baseVertices.emplace_back(plane.baseVertex);
                    drawCalls.emplace_back(plane.drawCall);

                    polyCounter += 2;

                    auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                    auto model = MatrixMath::translate({
                                                               pass.dstRect.position.x,
                                                               pass.dstRect.position.y,
                                                               0})
                                 * rotMat;

                    ShaderUniformBuffer buffer;

                    buffer.passData.mvp = viewProjectionMatrix * model;
                    auto color = pass.color.divide();
                    buffer.passData.color[0] = color.x;
                    buffer.passData.color[1] = color.y;
                    buffer.passData.color[2] = color.z;
                    buffer.passData.color[3] = color.w;

                    buffer.passData.colorMixFactor_alphaMixFactor_colorFactor[0] = pass.mix;
                    buffer.passData.colorMixFactor_alphaMixFactor_colorFactor[1] = pass.alphaMix;
                    buffer.passData.colorMixFactor_alphaMixFactor_colorFactor[2] = pass.colorFactor;
                    buffer.passData.texAtlasLevel_texAtlasIndex[0] = pass.texture.level;
                    buffer.passData.texAtlasLevel_texAtlasIndex[1] = (int) pass.texture.index;
                    auto uvOffset = pass.srcRect.position / pass.texture.size.convert<float>();
                    buffer.passData.uvOffset_uvScale[0] = uvOffset.x;
                    buffer.passData.uvOffset_uvScale[1] = uvOffset.y;
                    auto uvScale = (pass.srcRect.dimensions /
                                    pass.texture.size.convert<float>());
                    buffer.passData.uvOffset_uvScale[2] = uvScale.x;
                    buffer.passData.uvOffset_uvScale[3] = uvScale.y;
                    auto atlasScale = (pass.texture.size.convert<float>() /
                                       TextureAtlas::getResolutionLevelSize(
                                               pass.texture.level).convert<float>());
                    buffer.passData.atlasScale[0] = atlasScale.x;
                    buffer.passData.atlasScale[1] = atlasScale.y;

                    shaderBuffers.emplace_back(buffer);

                    break;
                }
            }
        }

        std::unordered_set<Vec2f> unusedPlanes;
        std::unordered_set<Vec2f> unusedSquares;
        std::unordered_set<std::pair<Vec2f, Vec2f>, LinePairHash> unusedLines;
        std::unordered_set<Vec2f> unusedPoints;
        std::unordered_set<std::pair<float, Vec2f>, RotationPairHash> unusedRotationMatrices;

        for (auto &pair: planeMeshes) {
            if (usedPlanes.find(pair.first) == usedPlanes.end()) {
                unusedPlanes.insert(pair.first);
            }
        }

        for (auto &pair: squareMeshes) {
            if (usedSquares.find(pair.first) == usedSquares.end()) {
                unusedSquares.insert(pair.first);
            }
        }

        for (auto &pair: lineMeshes) {
            if (usedLines.find(pair.first) == usedLines.end()) {
                unusedLines.insert(pair.first);
            }
        }

        for (auto &pair: pointMeshes) {
            if (usedPoints.find(pair.first) == usedPoints.end()) {
                unusedPoints.insert(pair.first);
            }
        }

        for (auto &pair: rotationMatrices) {
            if (usedRotationMatrices.find(pair.first) == usedRotationMatrices.end()) {
                unusedRotationMatrices.insert(pair.first);
            }
        }

        for (auto &v: unusedPlanes) {
            destroyPlane(v);
        }

        for (auto &v: unusedSquares) {
            destroySquare(v);
        }

        for (auto &v: unusedLines) {
            destroyLine(v.first, v.second);
        }

        for (auto &v: unusedPoints) {
            destroyPoint(v);
        }

        for (auto &v: unusedRotationMatrices) {
            rotationMatrices.erase(v);
        }

        mergeFreeVertexBufferRanges();
        mergeFreeIndexBufferRanges();

        updateVertexArrayObject();

        std::vector<DrawBatch> batches;
        DrawBatch currentBatch;
        for (auto y = 0; y < drawCalls.size(); y++) {
            auto prim = primitives.at(y);
            if (prim != currentBatch.primitive) {
                if (!currentBatch.drawCalls.empty())
                    batches.emplace_back(currentBatch);
                currentBatch = {};
                currentBatch.primitive = prim;
            }
            currentBatch.drawCalls.emplace_back(drawCalls.at(y));
            currentBatch.baseVertices.emplace_back(baseVertices.at(y));
            currentBatch.uniformBuffers.emplace_back(shaderBuffers[y]);
        }
        if (!currentBatch.drawCalls.empty())
            batches.emplace_back(currentBatch);

        renderPass->beginRenderPass(*userTarget, mViewportOffset, mViewportSize);
        renderPass->bindVertexArrayObject(*vertexArrayObject);


        ShaderBufferDesc shaderBufferDesc;
        shaderBufferDesc.size = sizeof(ShaderUniformBuffer);
        auto shaderBuffer = renderDevice.createShaderBuffer(shaderBufferDesc);

        renderPass->bindShaderData({
                                           *shaderBuffer,
                                           atlas.getBuffer(TEXTURE_ATLAS_8x8),
                                           atlas.getBuffer(TEXTURE_ATLAS_16x16),
                                           atlas.getBuffer(TEXTURE_ATLAS_32x32),
                                           atlas.getBuffer(TEXTURE_ATLAS_64x64),
                                           atlas.getBuffer(TEXTURE_ATLAS_128x128),
                                           atlas.getBuffer(TEXTURE_ATLAS_256x256),
                                           atlas.getBuffer(TEXTURE_ATLAS_512x512),
                                           atlas.getBuffer(TEXTURE_ATLAS_1024x1024),
                                           atlas.getBuffer(TEXTURE_ATLAS_2048x2048),
                                           atlas.getBuffer(TEXTURE_ATLAS_4096x4096),
                                           atlas.getBuffer(TEXTURE_ATLAS_8192x8192),
                                           atlas.getBuffer(TEXTURE_ATLAS_16384x16384),
                                   });

        renderPass->bindPipeline(*trianglePipeline);

        Primitive currentPrimitive = TRIANGLES;

        for (auto &batch: batches) {
            switch (batch.primitive) {
                case POINTS:
                    if (currentPrimitive != batch.primitive) {
                        currentPrimitive = batch.primitive;
                        renderPass->bindPipeline(*pointPipeline);
                    }
                    break;
                case LINES:
                    if (currentPrimitive != batch.primitive) {
                        currentPrimitive = batch.primitive;
                        renderPass->bindPipeline(*linePipeline);
                    }
                    break;
                case TRIANGLES:
                    if (currentPrimitive != batch.primitive) {
                        currentPrimitive = batch.primitive;
                        renderPass->bindPipeline(*trianglePipeline);
                    }
                    break;
                default:
                    throw std::runtime_error("Unsupported primitive");
            }

            for (auto y = 0; y < batch.drawCalls.size(); y++) {
                shaderBuffer->upload(batch.uniformBuffers.at(y));
                renderPass->drawIndexed(batch.drawCalls.at(y), batch.baseVertices.at(y));
            }
        }

        renderPass->endRenderPass();
    }

    void Renderer2D::presentMultiDraw() {
        int drawCycles = 0;
        if (passes.size() > MAX_MULTI_DRAW_COUNT) {
            drawCycles = passes.size() / MAX_MULTI_DRAW_COUNT;
            if (drawCycles * MAX_MULTI_DRAW_COUNT < passes.size()) {
                drawCycles += 1;
            }
        } else if (!passes.empty()) {
            drawCycles = 1;
        }

        for (int i = 0; i < drawCycles; i++) {
            usedPlanes.clear();
            usedSquares.clear();
            usedLines.clear();
            usedPoints.clear();
            usedRotationMatrices.clear();

            std::vector<RenderPass::DrawCall> drawCalls;
            std::vector<size_t> baseVertices;
            std::vector<Primitive> primitives;
            std::vector<PassData> passData(MAX_MULTI_DRAW_COUNT);

            auto currentPassBase = i * MAX_MULTI_DRAW_COUNT;
            for (auto passIndex = 0;
                 passIndex < MAX_MULTI_DRAW_COUNT && passIndex + currentPassBase < passes.size();
                 passIndex++) {
                auto &pass = passes.at(passIndex + currentPassBase);
                switch (pass.type) {
                    case Pass::COLOR_POINT: {
                        usedPoints.insert(pass.dstRect.position);
                        auto point = getPoint(pass.dstRect.position);

                        primitives.emplace_back(point.primitive);
                        baseVertices.emplace_back(point.baseVertex);
                        drawCalls.emplace_back(point.drawCall);

                        polyCounter += 1;

                        auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                        auto model = MatrixMath::translate({
                                                                   pass.srcRect.position.x,
                                                                   pass.srcRect.position.y,
                                                                   0})
                                     * rotMat;

                        passData[passIndex].mvp = viewProjectionMatrix * model;
                        auto color = pass.color.divide();
                        passData[passIndex].color[0] = color.x;
                        passData[passIndex].color[1] = color.y;
                        passData[passIndex].color[2] = color.z;
                        passData[passIndex].color[3] = color.w;
                        break;
                    }
                    case Pass::COLOR_LINE: {
                        usedLines.insert(std::make_pair(pass.dstRect.position, pass.dstRect.dimensions));

                        auto line = getLine(pass.dstRect.position, pass.dstRect.dimensions);

                        primitives.emplace_back(line.primitive);
                        baseVertices.emplace_back(line.baseVertex);
                        drawCalls.emplace_back(line.drawCall);

                        polyCounter += 1;

                        auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                        auto model = MatrixMath::translate({
                                                                   pass.srcRect.position.x,
                                                                   pass.srcRect.position.y,
                                                                   0})
                                     * rotMat;

                        passData[passIndex].mvp = viewProjectionMatrix * model;
                        auto color = pass.color.divide();
                        passData[passIndex].color[0] = color.x;
                        passData[passIndex].color[1] = color.y;
                        passData[passIndex].color[2] = color.z;
                        passData[passIndex].color[3] = color.w;
                        break;
                    }
                    case Pass::COLOR_PLANE: {
                        if (pass.fill) {
                            usedPlanes.insert(pass.dstRect.dimensions);
                            auto plane = getPlane(pass.dstRect.dimensions);

                            primitives.emplace_back(plane.primitive);
                            baseVertices.emplace_back(plane.baseVertex);
                            drawCalls.emplace_back(plane.drawCall);

                            polyCounter += 2;
                        } else {
                            usedSquares.insert(pass.dstRect.dimensions);
                            auto square = getSquare(pass.dstRect.dimensions);

                            primitives.emplace_back(square.primitive);
                            baseVertices.emplace_back(square.baseVertex);
                            drawCalls.emplace_back(square.drawCall);

                            polyCounter += 4;
                        }

                        auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                        auto model = MatrixMath::translate({
                                                                   pass.dstRect.position.x,
                                                                   pass.dstRect.position.y,
                                                                   0})
                                     * rotMat;

                        passData[passIndex].mvp = viewProjectionMatrix * model;
                        auto color = pass.color.divide();
                        passData[passIndex].color[0] = color.x;
                        passData[passIndex].color[1] = color.y;
                        passData[passIndex].color[2] = color.z;
                        passData[passIndex].color[3] = color.w;
                        break;
                    }
                    case Pass::TEXTURE: {
                        usedPlanes.insert(pass.dstRect.dimensions);
                        auto plane = getPlane(pass.dstRect.dimensions);

                        primitives.emplace_back(plane.primitive);
                        baseVertices.emplace_back(plane.baseVertex);
                        drawCalls.emplace_back(plane.drawCall);

                        polyCounter += 2;

                        auto rotMat = getRotationMatrix(pass.rotation, pass.center);

                        auto model = MatrixMath::translate({
                                                                   pass.dstRect.position.x,
                                                                   pass.dstRect.position.y,
                                                                   0})
                                     * rotMat;

                        passData[passIndex].mvp = viewProjectionMatrix * model;

                        auto color = pass.color.divide();
                        passData[passIndex].color[0] = color.x;
                        passData[passIndex].color[1] = color.y;
                        passData[passIndex].color[2] = color.z;
                        passData[passIndex].color[3] = color.w;
                        passData[passIndex].colorMixFactor_alphaMixFactor_colorFactor[0] = pass.mix;
                        passData[passIndex].colorMixFactor_alphaMixFactor_colorFactor[1] = pass.alphaMix;
                        passData[passIndex].colorMixFactor_alphaMixFactor_colorFactor[2] = pass.colorFactor;
                        passData[passIndex].texAtlasLevel_texAtlasIndex[0] = pass.texture.level;
                        passData[passIndex].texAtlasLevel_texAtlasIndex[1] = (int) pass.texture.index;
                        auto uvOffset = pass.srcRect.position / pass.texture.size.convert<float>();
                        passData[passIndex].uvOffset_uvScale[0] = uvOffset.x;
                        passData[passIndex].uvOffset_uvScale[1] = uvOffset.y;
                        auto uvScale = (pass.srcRect.dimensions /
                                        pass.texture.size.convert<float>());
                        passData[passIndex].uvOffset_uvScale[2] = uvScale.x;
                        passData[passIndex].uvOffset_uvScale[3] = uvScale.y;
                        auto atlasScale = (pass.texture.size.convert<float>() /
                                           TextureAtlas::getResolutionLevelSize(
                                                   pass.texture.level).convert<float>());
                        passData[passIndex].atlasScale[0] = atlasScale.x;
                        passData[passIndex].atlasScale[1] = atlasScale.y;
                        break;
                    }
                }
            }

            std::unordered_set<Vec2f> unusedPlanes;
            std::unordered_set<Vec2f> unusedSquares;
            std::unordered_set<std::pair<Vec2f, Vec2f>, LinePairHash> unusedLines;
            std::unordered_set<Vec2f> unusedPoints;
            std::unordered_set<std::pair<float, Vec2f>, RotationPairHash> unusedRotationMatrices;

            for (auto &pair: planeMeshes) {
                if (usedPlanes.find(pair.first) == usedPlanes.end()) {
                    unusedPlanes.insert(pair.first);
                }
            }

            for (auto &pair: squareMeshes) {
                if (usedSquares.find(pair.first) == usedSquares.end()) {
                    unusedSquares.insert(pair.first);
                }
            }

            for (auto &pair: lineMeshes) {
                if (usedLines.find(pair.first) == usedLines.end()) {
                    unusedLines.insert(pair.first);
                }
            }

            for (auto &pair: pointMeshes) {
                if (usedPoints.find(pair.first) == usedPoints.end()) {
                    unusedPoints.insert(pair.first);
                }
            }

            for (auto &pair: rotationMatrices) {
                if (usedRotationMatrices.find(pair.first) == usedRotationMatrices.end()) {
                    unusedRotationMatrices.insert(pair.first);
                }
            }

            for (auto &v: unusedPlanes) {
                destroyPlane(v);
            }

            for (auto &v: unusedSquares) {
                destroySquare(v);
            }

            for (auto &v: unusedLines) {
                destroyLine(v.first, v.second);
            }

            for (auto &v: unusedPoints) {
                destroyPoint(v);
            }

            for (auto &v: unusedRotationMatrices) {
                rotationMatrices.erase(v);
            }

            mergeFreeVertexBufferRanges();
            mergeFreeIndexBufferRanges();

            updateVertexArrayObject();

            std::vector<DrawBatchMultiDraw> batches;
            DrawBatchMultiDraw currentBatch;
            size_t currentBatchIndex = 0;
            for (auto y = 0; y < drawCalls.size(); y++) {
                auto prim = primitives.at(y);
                if (prim != currentBatch.primitive) {
                    if (!currentBatch.drawCalls.empty())
                        batches.emplace_back(currentBatch);
                    currentBatch = {};
                    currentBatch.primitive = prim;
                    currentBatchIndex = 0;
                }
                currentBatch.drawCalls.emplace_back(drawCalls.at(y));
                currentBatch.baseVertices.emplace_back(baseVertices.at(y));
                currentBatch.uniformBuffer.passes[currentBatchIndex++] = passData[y];
            }

            if (!currentBatch.drawCalls.empty())
                batches.emplace_back(currentBatch);

            renderPass->beginRenderPass(*userTarget, mViewportOffset, mViewportSize);
            renderPass->bindVertexArrayObject(*vertexArrayObject);

            ShaderBufferDesc shaderBufferDesc;
            shaderBufferDesc.size = sizeof(ShaderUniformBufferMultiDraw);
            auto shaderBuffer = renderDevice.createShaderBuffer(shaderBufferDesc);

            renderPass->bindShaderData({
                                               *shaderBuffer,
                                               atlas.getBuffer(TEXTURE_ATLAS_8x8),
                                               atlas.getBuffer(TEXTURE_ATLAS_16x16),
                                               atlas.getBuffer(TEXTURE_ATLAS_32x32),
                                               atlas.getBuffer(TEXTURE_ATLAS_64x64),
                                               atlas.getBuffer(TEXTURE_ATLAS_128x128),
                                               atlas.getBuffer(TEXTURE_ATLAS_256x256),
                                               atlas.getBuffer(TEXTURE_ATLAS_512x512),
                                               atlas.getBuffer(TEXTURE_ATLAS_1024x1024),
                                               atlas.getBuffer(TEXTURE_ATLAS_2048x2048),
                                               atlas.getBuffer(TEXTURE_ATLAS_4096x4096),
                                               atlas.getBuffer(TEXTURE_ATLAS_8192x8192),
                                               atlas.getBuffer(TEXTURE_ATLAS_16384x16384),
                                       });

            renderPass->bindPipeline(*trianglePipelineMultiDraw);
            Primitive currentPrimitive = TRIANGLES;

            for (auto &batch: batches) {
                shaderBuffer->upload(batch.uniformBuffer);

                switch (batch.primitive) {
                    case POINTS:
                        if (currentPrimitive != batch.primitive) {
                            currentPrimitive = batch.primitive;
                            renderPass->bindPipeline(*pointPipelineMultiDraw);
                        }
                        break;
                    case LINES:
                        if (currentPrimitive != batch.primitive) {
                            currentPrimitive = batch.primitive;
                            renderPass->bindPipeline(*linePipelineMultiDraw);
                        }
                        break;
                    case TRIANGLES:
                        if (currentPrimitive != batch.primitive) {
                            currentPrimitive = batch.primitive;
                            renderPass->bindPipeline(*trianglePipelineMultiDraw);
                        }
                        break;
                    default:
                        throw std::runtime_error("Unsupported primitive");
                }

                renderPass->multiDrawIndexed(batch.drawCalls, batch.baseVertices);
            }

            renderPass->endRenderPass();
        }
    }

    Renderer2D::MeshDrawData Renderer2D::getPlane(const Vec2f &size) {
        auto it = planeMeshes.find(size);
        if (it != planeMeshes.end()) {
            return it->second;
        } else {
            VertexStream vertexStream;

            vertexStream.addVertex(VertexBuilder()
                                           .addVec2(Vec2f(0, 0))
                                           .addVec2(Vec2f(0, 0))
                                           .build());
            vertexStream.addVertex(VertexBuilder()
                                           .addVec2(Vec2f(size.x, 0))
                                           .addVec2(Vec2f(1, 0))
                                           .build());
            vertexStream.addVertex(VertexBuilder()
                                           .addVec2(Vec2f(0, size.y))
                                           .addVec2(Vec2f(0, 1))
                                           .build());
            vertexStream.addVertex(VertexBuilder()
                                           .addVec2(Vec2f(size.x,
                                                          size.y))
                                           .addVec2(Vec2f(1, 1))
                                           .build());

            auto vertexBufferOffset = allocateVertexData(vertexStream.getVertexBuffer().size());

            vertexBuffer->upload(vertexBufferOffset,
                                 vertexStream.getVertexBuffer().data(),
                                 vertexStream.getVertexBuffer().size());

            std::vector<unsigned int> indices;
            indices.emplace_back(0);
            indices.emplace_back(1);
            indices.emplace_back(2);

            indices.emplace_back(1);
            indices.emplace_back(2);
            indices.emplace_back(3);

            auto indexBufferOffset = allocateIndexData(indices.size() * sizeof(unsigned int));

            auto drawCall = RenderPass::DrawCall{
                    .offset = indexBufferOffset,
                    .count = indices.size()};

            indexBuffer->upload(indexBufferOffset,
                                reinterpret_cast<const uint8_t *>(indices.data()),
                                indices.size() * sizeof(unsigned int));

            planeMeshes[size] = MeshDrawData{.primitive = TRIANGLES,
                    .drawCall = drawCall,
                    .baseVertex = vertexBufferOffset / vertexSize};

            return planeMeshes.at(size);
        }
    }

    Renderer2D::MeshDrawData Renderer2D::getSquare(const Vec2f &size) {
        auto it = squareMeshes.find(size);
        if (it != squareMeshes.end()) {
            return it->second;
        } else {
            VertexStream vertexStream;

            vertexStream.addVertex(VertexBuilder()
                                           .addVec2(Vec2f(0, 0))
                                           .addVec2(Vec2f(0, 0))
                                           .build());
            vertexStream.addVertex(VertexBuilder()
                                           .addVec2(Vec2f(size.x, 0))
                                           .addVec2(Vec2f(1, 0))
                                           .build());
            vertexStream.addVertex(VertexBuilder()
                                           .addVec2(Vec2f(0, size.y))
                                           .addVec2(Vec2f(0, 1))
                                           .build());
            vertexStream.addVertex(VertexBuilder()
                                           .addVec2(Vec2f(size.x,
                                                          size.y))
                                           .addVec2(Vec2f(1, 1))
                                           .build());

            auto vertexBufferOffset = allocateVertexData(vertexStream.getVertexBuffer().size());

            vertexBuffer->upload(vertexBufferOffset,
                                 vertexStream.getVertexBuffer().data(),
                                 vertexStream.getVertexBuffer().size());

            std::vector<unsigned int> indices;
            indices.emplace_back(0);
            indices.emplace_back(1);

            indices.emplace_back(1);
            indices.emplace_back(3);

            indices.emplace_back(3);
            indices.emplace_back(2);

            indices.emplace_back(2);
            indices.emplace_back(0);

            auto indexBufferOffset = allocateIndexData(indices.size() * sizeof(unsigned int));

            auto drawCall = RenderPass::DrawCall{
                    .offset = indexBufferOffset,
                    .count = indices.size()};

            indexBuffer->upload(indexBufferOffset,
                                reinterpret_cast<const uint8_t *>(indices.data()),
                                indices.size() * sizeof(unsigned int));

            squareMeshes[size] = MeshDrawData{.primitive = LINES,
                    .drawCall = drawCall,
                    .baseVertex = vertexBufferOffset / vertexSize};

            return squareMeshes.at(size);
        }

    }

    Renderer2D::MeshDrawData Renderer2D::getLine(const Vec2f &start, const Vec2f &end) {
        auto pair = std::make_pair(start, end);
        auto it = lineMeshes.find(pair);
        if (it != lineMeshes.end()) {
            return it->second;
        } else {
            VertexStream vertexStream;

            vertexStream.addVertex(VertexBuilder()
                                           .addVec2(start)
                                           .addVec2(Vec2f())
                                           .build());
            vertexStream.addVertex(VertexBuilder()
                                           .addVec2(end)
                                           .addVec2(Vec2f())
                                           .build());

            auto vertexBufferOffset = allocateVertexData(vertexStream.getVertexBuffer().size());

            vertexBuffer->upload(vertexBufferOffset,
                                 vertexStream.getVertexBuffer().data(),
                                 vertexStream.getVertexBuffer().size());

            std::vector<unsigned int> indices;
            indices.emplace_back(0);
            indices.emplace_back(1);

            auto indexBufferOffset = allocateIndexData(indices.size() * sizeof(unsigned int));

            auto drawCall = RenderPass::DrawCall{
                    .offset = indexBufferOffset,
                    .count = indices.size()};

            indexBuffer->upload(indexBufferOffset,
                                reinterpret_cast<const uint8_t *>(indices.data()),
                                indices.size() * sizeof(unsigned int));

            lineMeshes[pair] = MeshDrawData{.primitive = LINES,
                    .drawCall = drawCall,
                    .baseVertex = vertexBufferOffset / vertexSize};

            return lineMeshes.at(pair);
        }
    }

    Renderer2D::MeshDrawData Renderer2D::getPoint(const Vec2f &point) {
        auto it = pointMeshes.find(point);
        if (it != pointMeshes.end()) {
            return it->second;
        } else {
            VertexStream vertexStream;

            vertexStream.addVertex(VertexBuilder()
                                           .addVec2(point)
                                           .addVec2(Vec2f())
                                           .build());

            auto vertexBufferOffset = allocateVertexData(vertexStream.getVertexBuffer().size());

            vertexBuffer->upload(vertexBufferOffset,
                                 vertexStream.getVertexBuffer().data(),
                                 vertexStream.getVertexBuffer().size());

            std::vector<unsigned int> indices;
            indices.emplace_back(0);

            auto indexBufferOffset = allocateIndexData(indices.size() * sizeof(unsigned int));

            auto drawCall = RenderPass::DrawCall{
                    .offset = indexBufferOffset,
                    .count = indices.size()};

            indexBuffer->upload(indexBufferOffset,
                                reinterpret_cast<const uint8_t *>(indices.data()),
                                indices.size() * sizeof(unsigned int));

            pointMeshes[point] = MeshDrawData{.primitive = POINTS,
                    .drawCall = drawCall,
                    .baseVertex = vertexBufferOffset / vertexSize};

            return pointMeshes.at(point);
        }
    }

    void Renderer2D::destroyPlane(const Vec2f &size) {
        auto drawData = planeMeshes.at(size);
        planeMeshes.erase(size);
        deallocateVertexData(drawData.baseVertex * vertexSize);
        deallocateIndexData(drawData.drawCall.offset);
    }

    void Renderer2D::destroySquare(const Vec2f &size) {
        auto drawData = squareMeshes.at(size);
        squareMeshes.erase(size);
        deallocateVertexData(drawData.baseVertex * vertexSize);
        deallocateIndexData(drawData.drawCall.offset);
    }

    void Renderer2D::destroyLine(const Vec2f &start, const Vec2f &end) {
        auto pair = std::make_pair(start, end);
        auto drawData = lineMeshes.at(pair);
        lineMeshes.erase(pair);
        deallocateVertexData(drawData.baseVertex * vertexSize);
        deallocateIndexData(drawData.drawCall.offset);
    }

    void Renderer2D::destroyPoint(const Vec2f &point) {
        auto drawData = pointMeshes.at(point);
        pointMeshes.erase(point);
        deallocateVertexData(drawData.baseVertex * vertexSize);
        deallocateIndexData(drawData.drawCall.offset);
    }

    size_t Renderer2D::allocateVertexData(size_t size) {
        bool foundFreeRange = false;
        auto ret = 0UL;
        for (auto &range: freeVertexBufferRanges) {
            if (range.second >= size) {
                ret = range.first;
                foundFreeRange = true;
            }
        }

        if (foundFreeRange) {
            auto rangeSize = freeVertexBufferRanges.at(ret) -= size;
            freeVertexBufferRanges.erase(ret);
            if (rangeSize > 0) {
                freeVertexBufferRanges[ret + size] = rangeSize;
            }
        } else {
            ret = vertexBuffer->getDescription().size;
        }

        if (vertexBuffer->getDescription().size <= ret
            || vertexBuffer->getDescription().size < ret + size) {
            VertexBufferDesc desc;
            desc.size = vertexBuffer->getDescription().size + size;
            auto nBuffer = renderDevice.createVertexBuffer(desc);
            if (vertexBuffer->getDescription().size > 0) {
                nBuffer->copy(*vertexBuffer);
            }
            vertexBuffer = std::move(nBuffer);
            vaoChange = true;
        }

        allocatedVertexRanges[ret] = size;

        return ret;
    }

    void Renderer2D::deallocateVertexData(size_t offset) {
        auto size = allocatedVertexRanges.at(offset);
        allocatedVertexRanges.erase(offset);
        freeVertexBufferRanges[offset] = size;
    }

    size_t Renderer2D::allocateIndexData(size_t size) {
        bool foundFreeRange = false;
        auto ret = 0UL;
        for (auto &range: freeIndexBufferRanges) {
            if (range.second >= size) {
                ret = range.first;
                foundFreeRange = true;
            }
        }
        if (foundFreeRange) {
            auto rangeSize = freeIndexBufferRanges.at(ret) -= size;
            freeIndexBufferRanges.erase(ret);
            if (rangeSize > 0) {
                freeIndexBufferRanges[ret + size] = rangeSize;
            }
        } else {
            ret = indexBuffer->getDescription().size;
        }

        if (indexBuffer->getDescription().size <= ret
            || indexBuffer->getDescription().size <= ret + size) {
            IndexBufferDesc desc;
            desc.size = indexBuffer->getDescription().size + size;
            auto nBuffer = renderDevice.createIndexBuffer(desc);
            if (indexBuffer->getDescription().size > 0) {
                nBuffer->copy(*indexBuffer);
            }
            indexBuffer = std::move(nBuffer);
            vaoChange = true;
        }

        allocatedIndexRanges[ret] = size;

        return ret;
    }

    void Renderer2D::deallocateIndexData(size_t offset) {
        auto size = allocatedIndexRanges.at(offset);
        allocatedIndexRanges.erase(offset);
        freeIndexBufferRanges[offset] = size;
    }

    void Renderer2D::mergeFreeVertexBufferRanges() {
        bool merged = true;
        while (merged) {
            merged = false;
            auto vertexRanges = freeVertexBufferRanges;
            for (auto range = freeVertexBufferRanges.begin(); range != freeVertexBufferRanges.end(); range++) {
                auto next = range;
                next++;
                if (next != freeVertexBufferRanges.end()) {
                    if (range->first + range->second == next->first
                        && vertexRanges.find(range->first) != vertexRanges.end()
                        && vertexRanges.find(next->first) != vertexRanges.end()) {
                        merged = true;
                        vertexRanges.at(range->first) += next->second;
                        vertexRanges.erase(next->first);
                        range = next;
                    }
                }
            }
            freeVertexBufferRanges = vertexRanges;
        }
    }

    void Renderer2D::mergeFreeIndexBufferRanges() {
        bool merged = true;
        while (merged) {
            merged = false;
            auto indexRanges = freeIndexBufferRanges;
            for (auto range = freeIndexBufferRanges.begin(); range != freeIndexBufferRanges.end(); range++) {
                auto next = range;
                next++;
                if (next != freeIndexBufferRanges.end()) {
                    if (range->first + range->second == next->first
                        && indexRanges.find(range->first) != indexRanges.end()
                        && indexRanges.find(next->first) != indexRanges.end()) {
                        merged = true;
                        indexRanges.at(range->first) += next->second;
                        indexRanges.erase(next->first);
                        range = next;
                    }
                }
            }
            freeIndexBufferRanges = indexRanges;
        }
    }

    void Renderer2D::updateVertexArrayObject() {
        if (vaoChange) {
            vaoChange = false;
            vertexArrayObject->bindBuffers(*vertexBuffer, *indexBuffer);
        }
    }

    Mat4f Renderer2D::getRotationMatrix(float rotation, Vec2f center) {
        auto pair = std::make_pair(rotation, center);
        usedRotationMatrices.insert(pair);
        auto it = rotationMatrices.find(pair);
        if (it != rotationMatrices.end()) {
            return it->second;
        } else {
            rotationMatrices[pair] = MatrixMath::translate({
                                                                   center.x,
                                                                   center.y,
                                                                   0})
                                     * MatrixMath::rotate(Vec3f(0, 0, rotation))
                                     * MatrixMath::translate({
                                                                     -center.x,
                                                                     -center.y,
                                                                     0});
            return rotationMatrices.at(pair);
        }
    }
}