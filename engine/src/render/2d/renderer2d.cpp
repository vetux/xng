/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "asset/camera.hpp"
#include "render/2d/renderer2d.hpp"

#include "math/matrixmath.hpp"
#include "async/threadpool.hpp"
#include "shader/shadersource.hpp"

static const char *SHADER_VERT = R"###(#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 boneWeights;
layout (location = 7) in vec4 instanceRow0;
layout (location = 8) in vec4 instanceRow1;
layout (location = 9) in vec4 instanceRow2;
layout (location = 10) in vec4 instanceRow3;

layout (location = 0) out vec4 fPosition;
layout (location = 1) out vec2 fUv;

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    mat4 mvp;
    vec4 color;
} vars;

void main() {
    mat4 instanceMatrix;
    instanceMatrix[0] = instanceRow0;
    instanceMatrix[1] = instanceRow1;
    instanceMatrix[2] = instanceRow2;
    instanceMatrix[3] = instanceRow3;

    fPosition =  (vars.mvp * instanceMatrix) * vec4(position, 1);
    fUv = uv;
    gl_Position = fPosition;
}
)###";

static const char *SHADER_FRAG_COLOR = R"###(#version 420 core

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;

layout (location = 0) out vec4 color;

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    mat4 mvp;
    vec4 color;
} vars;

layout(binding = 1) uniform sampler2D diffuse;

void main() {
    color = vars.color;
}
)###";

static const char *SHADER_FRAG_TEXTURE = R"###(#version 420 core

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;

layout (location = 0) out vec4 color;

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    mat4 mvp;
    vec4 color;
} vars;

layout(binding = 1) uniform sampler2D diffuse;

void main() {
    color = texture(diffuse, fUv);
}
)###";

static const char *SHADER_FRAG_TEXT = R"###(#version 420 core

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;

layout (location = 0) out vec4 color;

layout(binding = 0, std140) uniform ShaderUniformBuffer
{
    mat4 mvp;
    vec4 color;
} vars;

layout(binding = 1) uniform sampler2D diffuse;

void main() {
    float grayscale = texture(diffuse, fUv).r;
    color = vars.color * vec4(grayscale, grayscale, grayscale, grayscale);
}
)###";

static const Transform cameraPosition = Transform({0, 0, 1}, Vec3f(), {});

static float distance(float val1, float val2) {
    float abs = val1 - val2;
    if (abs < 0)
        return abs * -1;
    else
        return abs;
}

namespace xng {
    struct ShaderUniformBuffer {
        Mat4f mvp = MatrixMath::identity();
        std::array<float, 4> color = Vec4f(1).getMemory();
    };

    /**
     * Get plane mesh with origin at top left offset by center and scaled in the y axis.
     *
     * @param size
     * @return
     */
    static Mesh createPlane(Vec2f size, Vec2f center, Rectf uvOffset, Vec2b flipUv) {
        Rectf scaledOffset(
                {uvOffset.position.x / size.x, uvOffset.position.y / size.y},
                {uvOffset.dimensions.x / size.x, uvOffset.dimensions.y / size.y});
        float uvNearX = scaledOffset.position.x;
        float uvFarX = scaledOffset.position.x + scaledOffset.dimensions.x;
        float uvNearY = scaledOffset.position.y;
        float uvFarY = scaledOffset.position.y + scaledOffset.dimensions.y;
        if (flipUv.y) {
            return Mesh(TRI, {
                    Vertex(Vec3f(0 - center.x, 0 - center.y, 0), {uvNearX, uvFarY}),
                    Vertex(Vec3f(size.x - center.x, 0 - center.y, 0), {uvFarX, uvFarY}),
                    Vertex(Vec3f(0 - center.x, size.y - center.y, 0), {uvNearX, uvNearY}),
                    Vertex(Vec3f(0 - center.x, size.y - center.y, 0), {uvNearX, uvNearY}),
                    Vertex(Vec3f(size.x - center.x, 0 - center.y, 0), {uvFarX, uvFarY}),
                    Vertex(Vec3f(size.x - center.x, size.y - center.y, 0), {uvFarX, uvNearY})
            });
        } else {
            return Mesh(TRI, {
                    Vertex(Vec3f(0 - center.x, 0 - center.y, 0), {uvNearX, uvNearY}),
                    Vertex(Vec3f(size.x - center.x, 0 - center.y, 0), {uvFarX, uvNearY}),
                    Vertex(Vec3f(0 - center.x, size.y - center.y, 0), {uvNearX, uvFarY}),
                    Vertex(Vec3f(0 - center.x, size.y - center.y, 0), {uvNearX, uvFarY}),
                    Vertex(Vec3f(size.x - center.x, 0 - center.y, 0), {uvFarX, uvNearY}),
                    Vertex(Vec3f(size.x - center.x, size.y - center.y, 0), {uvFarX, uvFarY})
            });
        }
    }

    static Mesh createSquare(Vec2f size, Vec2f center) {
        return Mesh(LINE, {
                Vertex(Vec3f(0 - center.x, 0 - center.y, 0)),
                Vertex(Vec3f(size.x - center.x, 0 - center.y, 0)),

                Vertex(Vec3f(size.x - center.x, 0 - center.y, 0)),
                Vertex(Vec3f(size.x - center.x, size.y - center.y, 0)),

                Vertex(Vec3f(size.x - center.x, size.y - center.y, 0)),
                Vertex(Vec3f(0 - center.x, size.y - center.y, 0)),

                Vertex(Vec3f(0 - center.x, size.y - center.y, 0)),
                Vertex(Vec3f(0 - center.x, 0 - center.y, 0))
        });
    }

    static Mesh createLine(Vec2f start, Vec2f end, Vec2f center) {
        return Mesh(LINE, {
                Vertex(Vec3f(start.x - center.x, start.y - center.y, 0)),
                Vertex(Vec3f(end.x - center.x, end.y - center.y, 0))
        });
    }

    Renderer2D::Renderer2D(RenderDevice &device, SPIRVCompiler &shaderCompiler, SPIRVDecompiler &shaderDecompiler)
            : renderDevice(device) {
        vs = ShaderSource(SHADER_VERT, "main", VERTEX, GLSL_420, false);
        fsColor = ShaderSource(SHADER_FRAG_COLOR, "main", FRAGMENT, GLSL_420, false);
        fsTexture = ShaderSource(SHADER_FRAG_TEXTURE, "main", FRAGMENT, GLSL_420, false);
        fsText = ShaderSource(SHADER_FRAG_TEXT, "main", FRAGMENT, GLSL_420, false);

        vs = vs.preprocess(shaderCompiler);
        fsColor = fsColor.preprocess(shaderCompiler);
        fsTexture = fsTexture.preprocess(shaderCompiler);
        fsText = fsText.preprocess(shaderCompiler);

        auto vsSrc = vs.compile(shaderCompiler);
        auto fsColSrc = fsColor.compile(shaderCompiler);
        auto fsTexSrc = fsTexture.compile(shaderCompiler);
        auto fsTextSrc = fsText.compile(shaderCompiler);

        colorShader = device.createShaderProgram(shaderDecompiler,
                                                 {.shaders = {{ShaderStage::VERTEX,   vsSrc.getShader()},
                                                              {ShaderStage::FRAGMENT, fsColSrc.getShader()}}});

        textureShader = device.createShaderProgram(shaderDecompiler,
                                                   {.shaders = {{ShaderStage::VERTEX,   vsSrc.getShader()},
                                                                {ShaderStage::FRAGMENT, fsTexSrc.getShader()}}});

        textShader = device.createShaderProgram(shaderDecompiler,
                                                {.shaders = {{ShaderStage::VERTEX,   vsSrc.getShader()},
                                                             {ShaderStage::FRAGMENT, fsTextSrc.getShader()}}});

        reallocatePipelines();
    }

    Renderer2D::~Renderer2D() = default;

    void Renderer2D::renderBegin(RenderTarget &target, bool clear, ColorRGBA clearColor) {
        renderBegin(target, clear, clearColor, {}, target.getDescription().size);
    }

    void Renderer2D::renderBegin(RenderTarget &target,
                                 bool clear,
                                 ColorRGBA clearColor,
                                 Vec2i viewportOffset,
                                 Vec2i viewportSize) {
        if (isRendering)
            throw std::runtime_error("Already rendering. ( Nested renderBegin calls? )");
        isRendering = true;

        userTarget = &target;

        this->viewportSize = viewportSize;
        if (this->clear != clear
            || this->clearColor != clearColor
            || this->viewportOffset != viewportOffset) {
            this->clear = clear;
            this->clearColor = clearColor;
            this->viewportOffset = viewportOffset;
            reallocatePipelines();
        }

        clearPipeline->setViewportSize(viewportSize);
        colorPipeline->setViewportSize(viewportSize);
        texturePipeline->setViewportSize(viewportSize);
        textPipeline->setViewportSize(viewportSize);

        setProjection({{}, viewportSize.convert<float>()});
    }

    void Renderer2D::renderPresent() {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");
        isRendering = false;

        clearPipeline->render(*userTarget, {});

        Pass::Type currentType = Pass::COLOR;
        std::vector<RenderPass> currentPasses;
        for (auto &pass: passes) {
            if (pass.type != currentType) {
                switch (currentType) {
                    case Pass::COLOR:
                        colorPipeline->render(*userTarget, currentPasses);
                        break;
                    case Pass::TEXTURE:
                        texturePipeline->render(*userTarget, currentPasses);
                        break;
                    case Pass::TEXT:
                        textPipeline->render(*userTarget, currentPasses);
                        break;
                }
                currentPasses.clear();
                currentType = pass.type;
            }
            currentPasses.emplace_back(pass.pass);
        }

        switch (currentType) {
            case Pass::COLOR:
                colorPipeline->render(*userTarget, currentPasses);
                break;
            case Pass::TEXTURE:
                texturePipeline->render(*userTarget, currentPasses);
                break;
            case Pass::TEXT:
                textPipeline->render(*userTarget, currentPasses);
                break;
        }

        userTarget = nullptr;

        std::unordered_set<PlaneDescription, PlaneDescriptionHashFunction> unusedPlanes;
        for (auto &pair: allocatedPlanes) {
            if (usedPlanes.find(pair.first) == usedPlanes.end()) {
                unusedPlanes.insert(pair.first);
            }
        }

        std::unordered_set<SquareDescription, SquareDescriptionHashFunction> unusedSquares;
        for (auto &pair: allocatedSquares) {
            if (usedSquares.find(pair.first) == usedSquares.end()) {
                unusedSquares.insert(pair.first);
            }
        }

        std::unordered_set<LineDescription, LineDescriptionHashFunction> unusedLines;
        for (auto &pair: allocatedLines) {
            if (usedLines.find(pair.first) == usedLines.end()) {
                unusedLines.insert(pair.first);
            }
        }

        std::unordered_set<Vec2f, Vector2HashFunction<float>> unusedPoints;
        for (auto &pair: allocatedPoints) {
            if (usedPoints.find(pair.first) == usedPoints.end()) {
                unusedPoints.insert(pair.first);
            }
        }

        for (auto &v: unusedPlanes)
            allocatedPlanes.erase(v);

        for (auto &v: unusedSquares)
            allocatedSquares.erase(v);

        for (auto &v: unusedLines)
            allocatedLines.erase(v);

        for (auto &v: unusedPoints)
            allocatedPoints.erase(v);

        usedPlanes.clear();
        usedSquares.clear();
        usedLines.clear();
        usedPoints.clear();

        allocatedInstancedMeshes.clear();

        passes.clear();

        shaderBuffers.resize(usedShaderBuffers);
        usedShaderBuffers = 0;
    }

    void Renderer2D::setProjection(const Rectf &projection) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        camera.type = ORTHOGRAPHIC;
        camera.left = projection.position.x;
        camera.right = projection.dimensions.x;
        camera.top = projection.position.y;
        camera.bottom = projection.dimensions.y;
    }

    void Renderer2D::draw(Rectf srcRect,
                          Rectf dstRect,
                          TextureBuffer &texture,
                          Vec2f center,
                          float rotation,
                          Vec2b flipUv) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        VertexBuffer &buffer = getPlane({dstRect.dimensions, center, srcRect, flipUv});

        Mat4f model = MatrixMath::identity();
        model = model * MatrixMath::translate(Vec3f(
                dstRect.position.x + center.x,
                dstRect.position.y + center.y,
                0));
        model = model * MatrixMath::rotate(Vec3f(0, 0, rotation));

        auto mvp = camera.projection() * camera.view(cameraPosition) * model;

        ShaderUniformBuffer shaderBufferUniform;
        shaderBufferUniform.mvp = mvp;

        auto &shaderBuffer = getShaderBuffer();
        shaderBuffer.upload(shaderBufferUniform);

        std::vector<RenderPass::ShaderBinding> bindings;
        bindings.emplace_back(RenderPass::ShaderBinding(shaderBuffer));
        bindings.emplace_back(RenderPass::ShaderBinding(texture));

        passes.emplace_back(Pass(Pass::TEXTURE, RenderPass(buffer, bindings)));
    }

    void Renderer2D::draw(Rectf dstRect, TextureBuffer &texture, Vec2f center, float rotation) {
        draw(Rectf({}, dstRect.dimensions), dstRect, texture, center, rotation);
    }

    void Renderer2D::draw(Rectf rectangle, ColorRGBA color, bool fill, Vec2f center, float rotation) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        Mesh mesh;

        VertexBuffer *buffer;
        if (fill)
            buffer = &getPlane({rectangle.dimensions, center, Rectf(Vec2f(), rectangle.dimensions), Vec2b(false)});
        else
            buffer = &getSquare({rectangle.dimensions, center});

        Mat4f modelMatrix = MatrixMath::identity();
        modelMatrix = modelMatrix * MatrixMath::translate(Vec3f(
                rectangle.position.x + center.x,
                rectangle.position.y + center.y,
                0));
        modelMatrix = modelMatrix * MatrixMath::rotate(Vec3f(0, 0, rotation));

        auto mvp = camera.projection() * camera.view(cameraPosition) * modelMatrix;

        ShaderUniformBuffer shaderBufferUniform;
        shaderBufferUniform.mvp = mvp;
        shaderBufferUniform.color = Vec4f((float) color.r() / 255,
                                          (float) color.g() / 255,
                                          (float) color.b() / 255,
                                          (float) color.a() / 255).getMemory();

        auto &shaderBuffer = getShaderBuffer();
        shaderBuffer.upload(shaderBufferUniform);

        std::vector<RenderPass::ShaderBinding> bindings;
        bindings.emplace_back(RenderPass::ShaderBinding(shaderBuffer));

        passes.emplace_back(Pass(Pass::COLOR, RenderPass(*buffer, bindings)));
    }

    void Renderer2D::draw(Vec2f start,
                          Vec2f end,
                          ColorRGBA color,
                          Vec2f center,
                          float rotation) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        VertexBuffer &buffer = getLine({start, end, center});

        Mat4f modelMatrix = MatrixMath::identity();
        modelMatrix = modelMatrix * MatrixMath::rotate(Vec3f(0, 0, rotation));

        auto mvp = camera.projection() * camera.view(cameraPosition) * modelMatrix;

        ShaderUniformBuffer shaderBufferUniform;
        shaderBufferUniform.mvp = mvp;
        shaderBufferUniform.color = Vec4f((float) color.r() / 255,
                                          (float) color.g() / 255,
                                          (float) color.b() / 255,
                                          (float) color.a() / 255).getMemory();

        auto &shaderBuffer = getShaderBuffer();
        shaderBuffer.upload(shaderBufferUniform);

        std::vector<RenderPass::ShaderBinding> bindings;
        bindings.emplace_back(RenderPass::ShaderBinding(shaderBuffer));

        passes.emplace_back(Pass(Pass::COLOR, RenderPass(buffer, bindings)));
    }

    void Renderer2D::draw(Vec2f point, ColorRGBA color) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        VertexBuffer &buffer = getPoint(point);

        Mat4f modelMatrix = MatrixMath::identity();
        auto mvp = camera.projection() * camera.view(cameraPosition) * modelMatrix;

        ShaderUniformBuffer shaderBufferUniform;
        shaderBufferUniform.mvp = mvp;
        shaderBufferUniform.color = Vec4f((float) color.r() / 255,
                                          (float) color.g() / 255,
                                          (float) color.b() / 255,
                                          (float) color.a() / 255).getMemory();

        auto &shaderBuffer = getShaderBuffer();
        shaderBuffer.upload(shaderBufferUniform);

        std::vector<RenderPass::ShaderBinding> bindings;
        bindings.emplace_back(RenderPass::ShaderBinding(shaderBuffer));

        passes.emplace_back(Pass(Pass::COLOR, RenderPass(buffer, bindings)));
    }

    void Renderer2D::draw(Text &text, Rectf dstRect, ColorRGBA color, Vec2f center, float rotation) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        auto srcRect = Rectf({}, text.getTexture().getDescription().size.convert<float>());

        VertexBuffer &buffer = getPlane({dstRect.dimensions, center, srcRect, Vec2b(false)});

        Mat4f model = MatrixMath::identity();
        model = model * MatrixMath::translate(Vec3f(
                dstRect.position.x + center.x,
                dstRect.position.y + center.y,
                0));
        model = model * MatrixMath::rotate(Vec3f(0, 0, rotation));

        auto mvp = camera.projection() * camera.view(cameraPosition) * model;

        ShaderUniformBuffer shaderBufferUniform;
        shaderBufferUniform.mvp = mvp;

        auto &shaderBuffer = getShaderBuffer();
        shaderBuffer.upload(shaderBufferUniform);

        std::vector<RenderPass::ShaderBinding> bindings;
        bindings.emplace_back(RenderPass::ShaderBinding(shaderBuffer));
        bindings.emplace_back(RenderPass::ShaderBinding(text.getTexture()));

        passes.emplace_back(Pass(Pass::TEXT, RenderPass(buffer, bindings)));
    }

    void Renderer2D::drawInstanced(const std::vector<std::pair<Vec2f, float>> &positions,
                                   Vec2f size,
                                   ColorRGBA color,
                                   bool fill,
                                   Vec2f center) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        Mesh mesh = createPlane(size, center, {}, Vec2b(false));

        std::vector<Mat4f> offsets;
        for (auto &p: positions) {
            Transform t;
            t.setPosition(Vec3f(
                    p.first.x + center.x,
                    p.first.y + center.y,
                    0));
            t.setRotation(Quaternion(Vec3f(0, 0, p.second)));
            offsets.emplace_back(t.model());
        }

        allocatedInstancedMeshes.emplace_back(renderDevice.createInstancedVertexBuffer(mesh, offsets));

        auto &meshBuffer = *allocatedInstancedMeshes.at(allocatedInstancedMeshes.size() - 1);

        Mat4f modelMatrix = MatrixMath::identity();

        auto mvp = camera.projection() * camera.view(cameraPosition) * modelMatrix;

        ShaderUniformBuffer shaderBufferUniform;
        shaderBufferUniform.mvp = mvp;
        shaderBufferUniform.color = Vec4f((float) color.r() / 255,
                                          (float) color.g() / 255,
                                          (float) color.b() / 255,
                                          (float) color.a() / 255).getMemory();

        auto &shaderBuffer = getShaderBuffer();
        shaderBuffer.upload(shaderBufferUniform);

        std::vector<RenderPass::ShaderBinding> bindings;
        bindings.emplace_back(RenderPass::ShaderBinding(shaderBuffer));

        passes.emplace_back(Pass(Pass::COLOR, RenderPass(meshBuffer, bindings)));
    }

    VertexBuffer &Renderer2D::getPlane(const Renderer2D::PlaneDescription &desc) {
        usedPlanes.insert(desc);
        auto it = allocatedPlanes.find(desc);
        if (it != allocatedPlanes.end()) {
            return *it->second;
        } else {
            auto mesh = createPlane(desc.size, desc.center, desc.uvOffset, desc.flipUv);
            allocatedPlanes[desc] = renderDevice.createInstancedVertexBuffer(mesh, {MatrixMath::identity()});
            return *allocatedPlanes[desc];
        }
    }

    VertexBuffer &Renderer2D::getSquare(const Renderer2D::SquareDescription &desc) {
        usedSquares.insert(desc);
        auto it = allocatedSquares.find(desc);
        if (it != allocatedSquares.end()) {
            return *it->second;
        } else {
            auto mesh = createSquare(desc.size, desc.center);
            allocatedSquares[desc] = renderDevice.createInstancedVertexBuffer(mesh, {MatrixMath::identity()});
            return *allocatedSquares[desc];
        }
    }

    VertexBuffer &Renderer2D::getLine(const Renderer2D::LineDescription &desc) {
        usedLines.insert(desc);
        auto it = allocatedLines.find(desc);
        if (it != allocatedLines.end()) {
            return *it->second;
        } else {
            auto mesh = createLine(desc.start, desc.end, desc.center);
            allocatedLines[desc] = renderDevice.createInstancedVertexBuffer(mesh, {MatrixMath::identity()});
            return *allocatedLines[desc];
        }
    }

    VertexBuffer &Renderer2D::getPoint(const Vec2f &point) {
        usedPoints.insert(point);
        auto it = allocatedPoints.find(point);
        if (it != allocatedPoints.end()) {
            return *it->second;
        } else {
            Mesh mesh(POINT, {
                    Vertex(Vec3f(point.x, point.y, 0))
            });
            allocatedPoints[point] = renderDevice.createInstancedVertexBuffer(mesh, {MatrixMath::identity()});
            return *allocatedPoints[point];
        }
    }

    ShaderBuffer &Renderer2D::getShaderBuffer() {
        if (shaderBuffers.size() <= usedShaderBuffers) {
            shaderBuffers.emplace_back(renderDevice.createShaderBuffer({.size = sizeof(ShaderUniformBuffer)}));
        }
        return *shaderBuffers.at(usedShaderBuffers++);
    }

    void Renderer2D::reallocatePipelines() {
        clearPipeline = renderDevice.createPipeline({.shader = *colorShader,
                                                            .viewportOffset = viewportOffset,
                                                            .viewportSize = viewportSize,
                                                            .multiSample = false,
                                                            .clearColorValue = clearColor,
                                                            .clearColor = clear,
                                                            .clearStencil = clear,
                                                            .enableDepthTest = false,
                                                            .enableBlending = true});

        colorPipeline = renderDevice.createPipeline({.shader = *colorShader,
                                                            .viewportOffset = viewportOffset,
                                                            .viewportSize = viewportSize,
                                                            .multiSample = false,
                                                            .clearColor = false,
                                                            .clearStencil = false,
                                                            .enableDepthTest = false,
                                                            .enableBlending = true});

        texturePipeline = renderDevice.createPipeline({.shader = *textureShader,
                                                              .viewportOffset = viewportOffset,
                                                              .viewportSize = viewportSize,
                                                              .multiSample = false,
                                                              .clearColor = false,
                                                              .clearStencil = false,
                                                              .enableDepthTest = false,
                                                              .enableBlending = true});

        textPipeline = renderDevice.createPipeline({.shader = *textShader,
                                                           .viewportOffset = viewportOffset,
                                                           .viewportSize = viewportSize,
                                                           .multiSample = false,
                                                           .clearColor = false,
                                                           .clearStencil = false,
                                                           .enableDepthTest = false,
                                                           .enableBlending = true});
    }
}