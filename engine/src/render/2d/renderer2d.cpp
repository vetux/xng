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

#include "asset/camera.hpp"
#include "render/2d/renderer2d.hpp"

#include "math/matrixmath.hpp"
#include "async/threadpool.hpp"
#include "render/shader/shadercompiler.hpp"

static const char *SHADER_VERT = R"###(#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec4 instanceRow0;
layout (location = 6) in vec4 instanceRow1;
layout (location = 7) in vec4 instanceRow2;
layout (location = 8) in vec4 instanceRow3;

layout (location = 0) out vec4 fPosition;
layout (location = 1) out vec2 fUv;

uniform mat4 MVP;
uniform float USE_TEXTURE;
uniform vec4 COLOR;

void main()
{
    mat4 instanceMatrix;
    instanceMatrix[0] = instanceRow0;
    instanceMatrix[1] = instanceRow1;
    instanceMatrix[2] = instanceRow2;
    instanceMatrix[3] = instanceRow3;

    fPosition = (MVP * instanceMatrix) * vec4(position, 1);
    fUv = uv;
    gl_Position = fPosition;
}
)###";

static const char *SHADER_FRAG = R"###(#version 410 core

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;

layout (location = 0) out vec4 color;

uniform mat4 MVP;
uniform float USE_TEXTURE;
uniform vec4 COLOR;

uniform sampler2D diffuse;

void main() {
    if (USE_TEXTURE != 0)
        color = texture(diffuse, fUv);
    else
        color = COLOR;
}
)###";

static const char *SHADER_TEXT_FRAG = R"###(#version 410 core

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;

layout (location = 0) out vec4 color;

uniform mat4 MVP;
uniform float USE_TEXTURE;
uniform vec4 COLOR;

uniform sampler2D diffuse;

void main() {
    float grayscale = texture(diffuse, fUv).r;
    color = COLOR * vec4(grayscale, grayscale, grayscale, grayscale);
}
)###";

static float distance(float val1, float val2) {
    float abs = val1 - val2;
    if (abs < 0)
        return abs * -1;
    else
        return abs;
}

namespace xengine {
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
            return Mesh(Mesh::TRI, {
                    Vertex(Vec3f(0 - center.x, 0 - center.y, 0), {uvNearX, uvFarY}),
                    Vertex(Vec3f(size.x - center.x, 0 - center.y, 0), {uvFarX, uvFarY}),
                    Vertex(Vec3f(0 - center.x, size.y - center.y, 0), {uvNearX, uvNearY}),
                    Vertex(Vec3f(0 - center.x, size.y - center.y, 0), {uvNearX, uvNearY}),
                    Vertex(Vec3f(size.x - center.x, 0 - center.y, 0), {uvFarX, uvFarY}),
                    Vertex(Vec3f(size.x - center.x, size.y - center.y, 0), {uvFarX, uvNearY})
            });
        } else {
            return Mesh(Mesh::TRI, {
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
        return Mesh(Mesh::LINE, {
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
        return Mesh(Mesh::LINE, {
                Vertex(Vec3f(start.x - center.x, start.y - center.y, 0)),
                Vertex(Vec3f(end.x - center.x, end.y - center.y, 0))
        });
    }

    Renderer2D::Renderer2D(RenderDevice &device)
            : renderDevice(device) {
        vs = ShaderSource(SHADER_VERT, "main", VERTEX, GLSL_410);
        fs = ShaderSource(SHADER_FRAG, "main", FRAGMENT, GLSL_410);
        fsText = ShaderSource(SHADER_TEXT_FRAG, "main", FRAGMENT, GLSL_410);

        vs.preprocess();
        fs.preprocess();
        fsText.preprocess();

        defShader = device.getAllocator().createShaderProgram(vs, fs);
        textShader = device.getAllocator().createShaderProgram(vs, fsText);
    }

    Renderer2D::~Renderer2D() = default;

    void Renderer2D::renderBegin(RenderTarget &target, bool clear, ColorRGBA clearColor) {
        renderDevice.getRenderer().renderBegin(target, RenderOptions({},
                                                                     target.getSize(),
                                                                     false,
                                                                     false,
                                                                     1,
                                                                     clearColor,
                                                                     1,
                                                                     clear, clear, clear));
        screenSize = target.getSize();
        setProjection({{}, screenSize.convert<float>()});
    }

    void Renderer2D::renderBegin(RenderTarget &target,
                                 bool clear,
                                 ColorRGBA clearColor,
                                 Vec2i viewportOffset,
                                 Vec2i viewportSize) {
        renderDevice.getRenderer().renderBegin(target, RenderOptions(viewportOffset,
                                                                     viewportSize,
                                                                     false,
                                                                     false,
                                                                     1,
                                                                     clearColor,
                                                                     1,
                                                                     clear, clear, clear));
        screenSize = viewportSize;
        setProjection({{}, screenSize.convert<float>()});
    }


    void Renderer2D::setProjection(const Rectf &projection) {
        camera.type = ORTHOGRAPHIC;
        camera.transform.setPosition({0, 0, 1});
        camera.left = projection.position.x;
        camera.right = projection.dimensions.x;
        camera.top = projection.position.y;
        camera.bottom = projection.dimensions.y;
    }

    void Renderer2D::draw(Rectf srcRect,
                          Rectf dstRect,
                          TextureBuffer &texture,
                          ShaderProgram &shader,
                          Vec2f center,
                          float rotation,
                          Vec2b flipUv,
                          bool alphaBlending) {
        MeshBuffer &buffer = getPlane({dstRect.dimensions, center, srcRect, flipUv});

        Mat4f model = MatrixMath::identity();
        model = model * MatrixMath::translate(Vec3f(
                dstRect.position.x + center.x,
                dstRect.position.y + center.y,
                0));
        model = model * MatrixMath::rotate(Vec3f(0, 0, rotation));

        auto mvp = camera.projection() * camera.view() * model;

        shader.activate();

        shader.setMat4("MVP", mvp);

        RenderCommand command(shader, buffer);
        command.textures.emplace_back(texture);
        command.properties.enableDepthTest = false;
        command.properties.enableBlending = alphaBlending;

        renderDevice.getRenderer().addCommand(command);
    }

    void Renderer2D::draw(Rectf srcRect,
                          Rectf dstRect,
                          TextureBuffer &texture,
                          Vec2f center,
                          float rotation,
                          Vec2b flipUv,
                          bool alphaBlending) {
        defShader->activate();
        defShader->setFloat("USE_TEXTURE", 1);
        defShader->setTexture("diffuse", 0);

        draw(srcRect, dstRect, texture, *defShader, center, rotation, flipUv, alphaBlending);
    }

    void Renderer2D::draw(Rectf dstRect, TextureBuffer &texture, Vec2f center, float rotation) {
        draw(Rectf({}, dstRect.dimensions), dstRect, texture, center, rotation);
    }

    void Renderer2D::draw(Rectf rectangle, ColorRGBA color, bool fill, Vec2f center, float rotation) {
        Mesh mesh;

        MeshBuffer *buffer;
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

        auto mvp = camera.projection() * camera.view() * modelMatrix;

        defShader->activate();
        defShader->setMat4("MVP", mvp);
        defShader->setFloat("USE_TEXTURE", 0);
        defShader->setVec4("COLOR", Vec4f((float) color.r() / 255,
                                          (float) color.g() / 255,
                                          (float) color.b() / 255,
                                          (float) color.a() / 255));

        RenderCommand command(*defShader, *buffer);
        command.properties.enableDepthTest = false;
        command.properties.enableBlending = true;

        renderDevice.getRenderer().addCommand(command);
    }

    void Renderer2D::draw(Vec2f start,
                          Vec2f end,
                          ColorRGBA color,
                          Vec2f center,
                          float rotation) {
        MeshBuffer &buffer = getLine({start, end, center});

        Mat4f modelMatrix = MatrixMath::identity();
        modelMatrix = modelMatrix * MatrixMath::rotate(Vec3f(0, 0, rotation));

        auto mvp = camera.projection() * camera.view() * modelMatrix;

        defShader->activate();
        defShader->setMat4("MVP", mvp);
        defShader->setFloat("USE_TEXTURE", 0);
        defShader->setVec4("COLOR", Vec4f((float) color.r() / 255,
                                          (float) color.g() / 255,
                                          (float) color.b() / 255,
                                          (float) color.a() / 255));

        RenderCommand command(*defShader, buffer);
        command.properties.enableDepthTest = false;
        command.properties.enableBlending = true;

        renderDevice.getRenderer().addCommand(command);
    }

    void Renderer2D::draw(Vec2f point, ColorRGBA color) {
        MeshBuffer &buffer = getPoint(point);

        Mat4f modelMatrix = MatrixMath::identity();
        auto mvp = camera.projection() * camera.view() * modelMatrix;

        defShader->activate();
        defShader->setMat4("MVP", mvp);
        defShader->setFloat("USE_TEXTURE", 0);
        defShader->setVec4("COLOR", Vec4f((float) color.r() / 255,
                                          (float) color.g() / 255,
                                          (float) color.b() / 255,
                                          (float) color.a() / 255));

        RenderCommand command(*defShader, buffer);
        command.properties.enableDepthTest = false;
        command.properties.enableBlending = true;

        renderDevice.getRenderer().addCommand(command);
    }

    void Renderer2D::draw(Text &text, Rectf dstRect, ColorRGBA color, Vec2f center, float rotation) {
        textShader->activate();
        textShader->setVec4("COLOR", Vec4f((float) color.r() / 255,
                                           (float) color.g() / 255,
                                           (float) color.b() / 255,
                                           (float) color.a() / 255));
        textShader->setTexture("diffuse", 0);

        auto srcRect = Rectf({}, text.getTexture().getAttributes().size.convert<float>());
        draw(srcRect,
             dstRect,
             text.getTexture(),
             *textShader,
             center,
             rotation);
    }

    void Renderer2D::draw(Text &text, Rectf dstRect, ShaderProgram &shader, Vec2f center, float rotation) {
        auto srcRect = Rectf({}, text.getTexture().getAttributes().size.convert<float>());
        draw(srcRect,
             dstRect,
             text.getTexture(),
             shader,
             center,
             rotation);
    }

    void Renderer2D::drawInstanced(const std::vector<std::pair<Vec2f, float>> &positions,
                                   Vec2f size,
                                   ColorRGBA color,
                                   bool fill,
                                   Vec2f center) {
        Mesh mesh = Mesh(Mesh::TRI, {
                Vertex(Vec3f(0 - center.x, 0 - center.y, 0), {0, 0}),
                Vertex(Vec3f(size.x - center.x, 0 - center.y, 0), {0, 0}),
                Vertex(Vec3f(0 - center.x, size.y - center.y, 0), {0, 0}),
                Vertex(Vec3f(0 - center.x, size.y - center.y, 0), {0, 0}),
                Vertex(Vec3f(size.x - center.x, 0 - center.y, 0), {0, 0}),
                Vertex(Vec3f(size.x - center.x, size.y - center.y, 0), {0, 0})
        });

        std::vector<Transform> offsets;
        for (auto &p: positions) {
            Transform t;
            t.setPosition(Vec3f(
                    p.first.x + center.x,
                    p.first.y + center.y,
                    0));
            t.setRotation(Quaternion(Vec3f(0, 0, p.second)));
            offsets.emplace_back(t);
        }

        allocatedInstancedMeshes.emplace_back(renderDevice.getAllocator().createInstancedMeshBuffer(mesh, offsets));

        auto &meshBuffer = *allocatedInstancedMeshes.at(allocatedInstancedMeshes.size() - 1);

        Mat4f modelMatrix = MatrixMath::identity();

        auto mvp = camera.projection() * camera.view() * modelMatrix;

        defShader->activate();
        defShader->setMat4("MVP", mvp);
        defShader->setFloat("USE_TEXTURE", 0);
        defShader->setVec4("COLOR", Vec4f((float) color.r() / 255,
                                          (float) color.g() / 255,
                                          (float) color.b() / 255,
                                          (float) color.a() / 255));

        RenderCommand command(*defShader, meshBuffer);
        command.properties.enableDepthTest = false;
        command.properties.enableBlending = true;

        renderDevice.getRenderer().addCommand(command);
    }

    void Renderer2D::renderPresent() {
        renderDevice.getRenderer().renderFinish();

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
    }

    MeshBuffer &Renderer2D::getPlane(const Renderer2D::PlaneDescription &desc) {
        usedPlanes.insert(desc);
        auto it = allocatedPlanes.find(desc);
        if (it != allocatedPlanes.end()) {
            return *it->second;
        } else {
            auto mesh = createPlane(desc.size, desc.center, desc.uvOffset, desc.flipUv);
            allocatedPlanes[desc] = renderDevice.getAllocator().createMeshBuffer(mesh);
            return *allocatedPlanes[desc];
        }
    }

    MeshBuffer &Renderer2D::getSquare(const Renderer2D::SquareDescription &desc) {
        usedSquares.insert(desc);
        auto it = allocatedSquares.find(desc);
        if (it != allocatedSquares.end()) {
            return *it->second;
        } else {
            auto mesh = createSquare(desc.size, desc.center);
            allocatedSquares[desc] = renderDevice.getAllocator().createMeshBuffer(mesh);
            return *allocatedSquares[desc];
        }
    }

    MeshBuffer &Renderer2D::getLine(const Renderer2D::LineDescription &desc) {
        usedLines.insert(desc);
        auto it = allocatedLines.find(desc);
        if (it != allocatedLines.end()) {
            return *it->second;
        } else {
            auto mesh = createLine(desc.start, desc.end, desc.center);
            allocatedLines[desc] = renderDevice.getAllocator().createMeshBuffer(mesh);
            return *allocatedLines[desc];
        }
    }

    MeshBuffer &Renderer2D::getPoint(const Vec2f &point) {
        usedPoints.insert(point);
        auto it = allocatedPoints.find(point);
        if (it != allocatedPoints.end()) {
            return *it->second;
        } else {
            Mesh mesh(Mesh::POINT, {
                    Vertex(Vec3f(point.x, point.y, 0))
            });
            allocatedPoints[point] = renderDevice.getAllocator().createMeshBuffer(mesh);
            return *allocatedPoints[point];
        }
    }
}