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

#include <utility>

#include "math/matrixmath.hpp"
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
        return Mesh(TRI, {
                Vertex(Vec3f(0 - center.x, 0 - center.y, 0),
                       {flipUv.x ? uvFarX : uvNearX, flipUv.y ? uvFarY : uvNearY}),
                Vertex(Vec3f(size.x - center.x, 0 - center.y, 0),
                       {flipUv.x ? uvNearX : uvFarX, flipUv.y ? uvFarY : uvNearY}),
                Vertex(Vec3f(0 - center.x, size.y - center.y, 0),
                       {flipUv.x ? uvFarX : uvNearX, flipUv.y ? uvNearY : uvFarY}),
                Vertex(Vec3f(0 - center.x, size.y - center.y, 0),
                       {flipUv.x ? uvFarX : uvNearX, flipUv.y ? uvNearY : uvFarY}),
                Vertex(Vec3f(size.x - center.x, 0 - center.y, 0),
                       {flipUv.x ? uvNearX : uvFarX, flipUv.y ? uvFarY : uvNearY}),
                Vertex(Vec3f(size.x - center.x, size.y - center.y, 0),
                       {flipUv.x ? uvNearX : uvFarX, flipUv.y ? uvNearY : uvFarY})
        });
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
            || this->clearColor != clearColor) {
            this->clear = clear;
            this->clearColor = clearColor;
            this->viewportOffset = viewportOffset;
            reallocatePipelines();
        }

        clearPipeline->setViewport(viewportOffset, viewportSize);
        colorPipeline->setViewport(viewportOffset, viewportSize);
        texturePipeline->setViewport(viewportOffset, viewportSize);
        textPipeline->setViewport(viewportOffset, viewportSize);

        setProjection({{}, viewportSize.convert<float>()});
        setCameraPosition({});
    }

    void Renderer2D::renderPresent() {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");
        isRendering = false;

        clearPipeline->render(*userTarget, {});

        std::vector<std::pair<Pass::Type, RenderCommand>> commands;
//TODO: Instance commands with identical geometry and shading
        for (auto &pass: passes) {
            RenderCommand command;
            switch (pass.type) {
                case Pass::COLOR: {
                    auto &vb = getPoly(std::get<std::vector<Vec2f>>(pass.geometry));

                    Mat4f modelMatrix = MatrixMath::identity();
                    modelMatrix = modelMatrix * MatrixMath::translate(Vec3f(
                            pass.position.x,
                            pass.position.y,
                            0));
                    modelMatrix = modelMatrix * MatrixMath::rotate(Vec3f(0, 0, pass.rotation));

                    auto mvp = camera.projection() * camera.view(cameraTransform) * modelMatrix;

                    ShaderUniformBuffer shaderBufferUniform;
                    shaderBufferUniform.mvp = mvp;
                    shaderBufferUniform.color = Vec4f((float) pass.color.r() / 255,
                                                      (float) pass.color.g() / 255,
                                                      (float) pass.color.b() / 255,
                                                      (float) pass.color.a() / 255).getMemory();

                    auto &shaderBuffer = getShaderBuffer();
                    shaderBuffer.upload(shaderBufferUniform);

                    std::vector<ShaderBinding> bindings;
                    bindings.emplace_back(ShaderBinding(shaderBuffer));

                    command = RenderCommand(vb, bindings);
                }
                    break;
                case Pass::COLOR_SOLID: {
                    auto &vb = getPlane(std::get<PlaneDescription>(pass.geometry));

                    Mat4f modelMatrix = MatrixMath::identity();
                    modelMatrix = modelMatrix * MatrixMath::translate(Vec3f(
                            pass.position.x,
                            pass.position.y,
                            0));
                    modelMatrix = modelMatrix * MatrixMath::rotate(Vec3f(0, 0, pass.rotation));

                    auto mvp = camera.projection() * camera.view(cameraTransform) * modelMatrix;

                    ShaderUniformBuffer shaderBufferUniform;
                    shaderBufferUniform.mvp = mvp;
                    shaderBufferUniform.color = Vec4f((float) pass.color.r() / 255,
                                                      (float) pass.color.g() / 255,
                                                      (float) pass.color.b() / 255,
                                                      (float) pass.color.a() / 255).getMemory();

                    auto &shaderBuffer = getShaderBuffer();
                    shaderBuffer.upload(shaderBufferUniform);

                    std::vector<ShaderBinding> bindings;
                    bindings.emplace_back(ShaderBinding(shaderBuffer));

                    command = RenderCommand(vb, bindings);
                }
                    break;
                case Pass::TEXTURE: {
                    auto &vb = getPlane(std::get<PlaneDescription>(pass.geometry));

                    Mat4f model = MatrixMath::identity();
                    model = model * MatrixMath::translate(Vec3f(
                            pass.position.x,
                            pass.position.y,
                            0));
                    model = model * MatrixMath::rotate(Vec3f(0, 0, pass.rotation));

                    auto mvp = camera.projection() * camera.view(cameraTransform) * model;

                    ShaderUniformBuffer shaderBufferUniform;
                    shaderBufferUniform.mvp = mvp;

                    auto &shaderBuffer = getShaderBuffer();
                    shaderBuffer.upload(shaderBufferUniform);

                    std::vector<ShaderBinding> bindings;
                    bindings.emplace_back(ShaderBinding(shaderBuffer));
                    bindings.emplace_back(ShaderBinding(*pass.texture));

                    command = RenderCommand(vb, bindings);
                }
                case Pass::TEXT: {
                    auto &vb = getPlane(std::get<PlaneDescription>(pass.geometry));

                    Mat4f model = MatrixMath::identity();
                    model = model * MatrixMath::translate(Vec3f(
                            pass.position.x,
                            pass.position.y,
                            0));
                    model = model * MatrixMath::rotate(Vec3f(0, 0, pass.rotation));

                    auto mvp = camera.projection() * camera.view(cameraTransform) * model;

                    ShaderUniformBuffer shaderBufferUniform;
                    shaderBufferUniform.mvp = mvp;
                    shaderBufferUniform.color = pass.color.divide().getMemory();

                    auto &shaderBuffer = getShaderBuffer();
                    shaderBuffer.upload(shaderBufferUniform);

                    std::vector<ShaderBinding> bindings;
                    bindings.emplace_back(ShaderBinding(shaderBuffer));
                    bindings.emplace_back(ShaderBinding(*pass.texture));

                    command = RenderCommand(vb, bindings);
                }
                    break;
            }
            commands.emplace_back(std::make_pair(pass.type, command));
        }

        Pass::Type currentType = Pass::COLOR;

        std::vector<RenderCommand> currentPasses;
        for (auto &cmd: commands) {
            if (cmd.first != currentType) {
                switch (currentType) {
                    case Pass::COLOR:
                    case Pass::COLOR_SOLID:
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
                currentType = cmd.first;
            }
            currentPasses.emplace_back(cmd.second);
        }

        switch (currentType) {
            case Pass::COLOR:
            case Pass::COLOR_SOLID:
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

        std::unordered_set<std::vector<Vec2f>, PolyHashFunction<float>> unusedPolys;
        for (auto &pair: allocatedPolys) {
            if (usedPolys.find(pair.first) == usedPolys.end()) {
                unusedPolys.insert(pair.first);
            }
        }

        for (auto &v: unusedPolys)
            allocatedPolys.erase(v);
        for (auto &v: unusedPlanes)
            allocatedPlanes.erase(v);

        usedPlanes.clear();
        usedPolys.clear();

        passes.clear();

        shaderBuffers.resize(usedShaderBuffers);
        usedShaderBuffers = 0;
    }

    void Renderer2D::renderClear(RenderTarget &target, ColorRGBA clearColor, Vec2i viewportOffset, Vec2i viewportSize) {
        if (!this->clear
            || this->clearColor != clearColor) {
            this->clear = true;
            this->clearColor = clearColor;
            this->viewportOffset = viewportOffset;
            this->viewportSize = viewportSize;
            reallocatePipelines();
        }

        clearPipeline->setViewport(viewportOffset, viewportSize);

        clearPipeline->render(target, {});
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

    void Renderer2D::setCameraPosition(const Vec2f &pos) {
        cameraTransform.setPosition({pos.x, pos.y, 1});
    }

    void Renderer2D::draw(Rectf srcRect,
                          Rectf dstRect,
                          TextureBuffer &texture,
                          Vec2f center,
                          float rotation,
                          Vec2b flipUv) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        PlaneDescription desc({dstRect.dimensions, center, srcRect, flipUv});
        passes.emplace_back(Pass(dstRect.position, rotation, desc, texture));
    }

    void Renderer2D::draw(Rectf dstRect, TextureBuffer &texture, Vec2f center, float rotation) {
        draw(Rectf({}, dstRect.dimensions), dstRect, texture, std::move(center), rotation);
    }

    void Renderer2D::draw(std::vector<Vec2f> poly,
                          Vec2f position,
                          ColorRGBA color,
                          Vec2f center,
                          float rotation) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");
        for (auto &vec: poly)
            vec += center;
        passes.emplace_back(Pass(position, rotation, poly, color));
    }

    void Renderer2D::draw(Rectf rectangle, ColorRGBA color, bool fill, Vec2f center, float rotation) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        if (fill)
            passes.emplace_back(Pass(rectangle.position,
                                     rotation,
                                     {rectangle.dimensions, center, Rectf(Vec2f(), rectangle.dimensions), Vec2b(false)},
                                     color));
        else
            passes.emplace_back(Pass(rectangle.position,
                                     rotation,
                                     getSquare({rectangle.dimensions, center}),
                                     color));
    }

    void Renderer2D::draw(Vec2f start,
                          Vec2f end,
                          ColorRGBA color) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        passes.emplace_back(Pass({}, 0, std::vector<Vec2f>({std::move(start), std::move(end)}), color));
    }

    void Renderer2D::draw(Vec2f point, ColorRGBA color) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        passes.emplace_back(Pass({}, 0, std::vector<Vec2f>({std::move(point)}), color));
    }

    void Renderer2D::draw(Text &text, Rectf dstRect, ColorRGBA color, Vec2f center, float rotation) {
        if (!isRendering)
            throw std::runtime_error("Not rendering. ( Nested renderBegin calls? )");

        auto srcRect = Rectf({}, text.getTexture().getDescription().size.convert<float>());
        passes.emplace_back(Pass(dstRect.position,
                                 rotation,
                                 {dstRect.dimensions, center, srcRect, Vec2b(false)},
                                 text,
                                 color));
    }

    VertexBuffer &Renderer2D::getPoly(const std::vector<Vec2f> &poly) {
        usedPolys.insert(poly);
        auto it = allocatedPolys.find(poly);
        if (it != allocatedPolys.end()) {
            return *it->second;
        } else {
            if (poly.size() < 2) {
                throw std::runtime_error("Invalid polygon vertices size");
            }
            std::vector<Vertex> vertices;
            for (auto i = 0; i < poly.size() - 1; i++) {
                auto &vec = poly.at(i);
                auto &nVec = poly.at(i + 1);
                vertices.emplace_back(Vertex(Vec3f(vec.x, vec.y, 0)));
                vertices.emplace_back(Vertex(Vec3f(nVec.x, nVec.y, 0)));
            }
            vertices.emplace_back(Vertex(Vec3f(poly.at(0).x, poly.at(0).y, 0)));
            vertices.emplace_back(
                    Vertex(Vec3f(poly.at(poly.size() - 1).x, poly.at(poly.size() - 1).y, 0)));
            auto mesh = Mesh(Primitive::LINE, vertices);
            allocatedPolys[poly] = renderDevice.createInstancedVertexBuffer(mesh, {MatrixMath::identity()});
            return *allocatedPolys[poly];
        }
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

    std::vector<Vec2f> Renderer2D::getSquare(const Renderer2D::SquareDescription &desc) {
        auto mesh = createSquare(desc.size, desc.center);
        std::vector<Vec2f> ret;
        if (mesh.indices.empty()) {
            for (auto &vert: mesh.vertices) {
                ret.emplace_back(Vec2f{vert.position().x, vert.position().y});
            }
        } else {
            for (auto &index: mesh.indices) {
                auto &vert = mesh.vertices.at(index);
                ret.emplace_back(Vec2f{vert.position().x, vert.position().y});
            }
        }
        return ret;
    }

    ShaderBuffer &Renderer2D::getShaderBuffer() {
        if (shaderBuffers.size() <= usedShaderBuffers) {
            shaderBuffers.emplace_back(renderDevice.createShaderBuffer({.size = sizeof(ShaderUniformBuffer)}));
        }
        return *shaderBuffers.at(usedShaderBuffers++);
    }

    void Renderer2D::reallocatePipelines() {
        clearPipeline = renderDevice.createRenderPipeline({.shader = *colorShader,
                                                                  .viewportOffset = viewportOffset,
                                                                  .viewportSize = viewportSize,
                                                                  .multiSample = false,
                                                                  .clearColorValue = clearColor,
                                                                  .clearColor = clear,
                                                                  .enableDepthTest = false,
                                                                  .enableBlending = true});

        colorPipeline = renderDevice.createRenderPipeline({.shader = *colorShader,
                                                                  .viewportOffset = viewportOffset,
                                                                  .viewportSize = viewportSize,
                                                                  .multiSample = false,
                                                                  .clearColor = false,
                                                                  .enableDepthTest = false,
                                                                  .enableBlending = true});

        texturePipeline = renderDevice.createRenderPipeline({.shader = *textureShader,
                                                                    .viewportOffset = viewportOffset,
                                                                    .viewportSize = viewportSize,
                                                                    .multiSample = false,
                                                                    .clearColor = false,
                                                                    .enableDepthTest = false,
                                                                    .enableBlending = true});

        textPipeline = renderDevice.createRenderPipeline({.shader = *textShader,
                                                                 .viewportOffset = viewportOffset,
                                                                 .viewportSize = viewportSize,
                                                                 .multiSample = false,
                                                                 .clearColor = false,
                                                                 .enableDepthTest = false,
                                                                 .enableBlending = true});
    }
}