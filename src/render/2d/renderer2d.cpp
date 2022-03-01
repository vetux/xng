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

#include "render/camera.hpp"
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

uniform mat4 MODEL_MATRIX;
uniform float USE_TEXTURE;
uniform vec4 COLOR;

void main()
{
    fPosition = MODEL_MATRIX * vec4(position, 1);
    fUv = uv;
    gl_Position = fPosition;
}
)###";

static const char *SHADER_FRAG = R"###(#version 410 core

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;

layout (location = 0) out vec4 color;

uniform mat4 MODEL_MATRIX;
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

uniform mat4 MODEL_MATRIX;
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
    static Mesh getPlane(Vec2f size, Vec2f center, Rectf uvOffset) {
        Rectf scaledOffset(
                {uvOffset.position.x / size.x, uvOffset.position.y / size.y},
                {uvOffset.dimensions.x / size.x, uvOffset.dimensions.y / size.y});
        float uvNearX = scaledOffset.position.x;
        float uvFarX = scaledOffset.position.x + scaledOffset.dimensions.x;
        float uvNearY = scaledOffset.position.y;
        float uvFarY = scaledOffset.position.y + scaledOffset.dimensions.y;
        return Mesh(Mesh::TRI, {
                Vertex(Vec3f(0 - center.x, 0 - center.y, 0), {uvNearX, uvNearY}),
                Vertex(Vec3f(size.x - center.x, 0 - center.y, 0), {uvFarX, uvNearY}),
                Vertex(Vec3f(0 - center.x, size.y - center.y, 0), {uvNearX, uvFarY}),
                Vertex(Vec3f(0 - center.x, size.y - center.y, 0), {uvNearX, uvFarY}),
                Vertex(Vec3f(size.x - center.x, 0 - center.y, 0), {uvFarX, uvNearY}),
                Vertex(Vec3f(size.x - center.x, size.y - center.y, 0), {uvFarX, uvFarY})
        });
    }

    static Mesh getSquare(Vec2f size, Vec2f center) {
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

    static Mesh getLine(Vec2f start, Vec2f end, Vec2f center) {
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

        defaultShader = device.getAllocator().createShaderProgram(vs, fs);
        defaultTextShader = device.getAllocator().createShaderProgram(vs, fsText);
    }

    Renderer2D::~Renderer2D() = default;

    void Renderer2D::renderBegin(RenderTarget &target, bool clear) {
        renderDevice.getRenderer().renderBegin(target, RenderOptions({},
                                                                     target.getSize(),
                                                                     true,
                                                                     false,
                                                                     1,
                                                                     {},
                                                                     1,
                                                                     clear, clear, clear));
        screenSize = target.getSize();
        setProjection({{}, screenSize.convert<float>()});
    }

    void Renderer2D::renderBegin(RenderTarget &target,
                                 bool clear,
                                 Vec2i viewportOffset,
                                 Vec2i viewportSize,
                                 ColorRGBA clearColor) {
        renderDevice.getRenderer().renderBegin(target, RenderOptions(viewportOffset,
                                                                     viewportSize,
                                                                     true,
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
                          float rotation) {
        Mesh mesh = getPlane(dstRect.dimensions, center, srcRect);

        MeshBuffer &buffer = **allocatedMeshes.insert(renderDevice.getAllocator().createMeshBuffer(mesh)).first;

        Mat4f modelMatrix = MatrixMath::identity();
        modelMatrix = modelMatrix * MatrixMath::translate(Vec3f(
                dstRect.position.x + center.x,
                dstRect.position.y + center.y,
                0));
        modelMatrix = modelMatrix * MatrixMath::rotate(Vec3f(0, 0, rotation));

        modelMatrix = camera.projection() * camera.view() * modelMatrix;

        shader.activate();
        shader.setMat4("MODEL_MATRIX", modelMatrix);
        shader.setFloat("USE_TEXTURE", 1);
        shader.setVec4("COLOR", Vec4f(1, 1, 1, 1));

        shader.setTexture("diffuse", 0);

        RenderCommand command(shader, buffer);
        command.textures.emplace_back(texture);
        command.properties.enableDepthTest = false;
        command.properties.enableBlending = true;

        renderDevice.getRenderer().addCommand(command);
    }

    void Renderer2D::draw(Rectf srcRect, Rectf dstRect, TextureBuffer &texture, Vec2f center, float rotation) {
        draw(srcRect, dstRect, texture, *defaultShader, center, rotation);
    }

    void Renderer2D::draw(Rectf dstRect, TextureBuffer &texture, Vec2f center, float rotation) {
        draw(Rectf({}, dstRect.dimensions), dstRect, texture, center, rotation);
    }

    void Renderer2D::draw(Rectf rectangle, ColorRGBA color, bool fill, Vec2f center, float rotation) {
        Mesh mesh;

        if (fill)
            mesh = getPlane(rectangle.dimensions, center, Rectf(Vec2f(), rectangle.dimensions));
        else
            mesh = getSquare(rectangle.dimensions, center);

        MeshBuffer &buffer = **allocatedMeshes.insert(renderDevice.getAllocator().createMeshBuffer(mesh)).first;

        Mat4f modelMatrix = MatrixMath::identity();
        modelMatrix = modelMatrix * MatrixMath::translate(Vec3f(
                rectangle.position.x + center.x,
                rectangle.position.y + center.y,
                0));
        modelMatrix = modelMatrix * MatrixMath::rotate(Vec3f(0, 0, rotation));

        modelMatrix = camera.projection() * camera.view() * modelMatrix;

        defaultShader->activate();
        defaultShader->setMat4("MODEL_MATRIX", modelMatrix);
        defaultShader->setFloat("USE_TEXTURE", 0);
        defaultShader->setVec4("COLOR", Vec4f((float) color.r() / 255,
                                              (float) color.g() / 255,
                                              (float) color.b() / 255,
                                              (float) color.a() / 255));

        RenderCommand command(*defaultShader, buffer);
        command.properties.enableDepthTest = false;
        command.properties.enableBlending = true;

        renderDevice.getRenderer().addCommand(command);
    }

    void Renderer2D::draw(Vec2f start,
                          Vec2f end,
                          ColorRGBA color,
                          Vec2f center,
                          float rotation) {
        Mesh mesh = getLine(start, end, center);

        auto it = allocatedMeshes.insert(renderDevice.getAllocator().createMeshBuffer(mesh));

        MeshBuffer &buffer = **it.first;

        Mat4f modelMatrix = MatrixMath::identity();
        modelMatrix = modelMatrix * MatrixMath::rotate(Vec3f(0, 0, rotation));

        modelMatrix = camera.projection() * camera.view() * modelMatrix;

        defaultShader->activate();
        defaultShader->setMat4("MODEL_MATRIX", modelMatrix);
        defaultShader->setFloat("USE_TEXTURE", 0);
        defaultShader->setVec4("COLOR", Vec4f((float) color.r() / 255,
                                              (float) color.g() / 255,
                                              (float) color.b() / 255,
                                              (float) color.a() / 255));

        RenderCommand command(*defaultShader, buffer);
        command.properties.enableDepthTest = false;
        command.properties.enableBlending = true;

        renderDevice.getRenderer().addCommand(command);
    }

    void Renderer2D::draw(Vec2f point, ColorRGBA color) {
        Mesh mesh(Mesh::POINT, {
                Vertex(Vec3f(point.x, point.y, 0))
        });

        MeshBuffer &buffer = **allocatedMeshes.insert(renderDevice.getAllocator().createMeshBuffer(mesh)).first;

        Mat4f modelMatrix = MatrixMath::identity();
        modelMatrix = camera.projection() * camera.view() * modelMatrix;

        defaultShader->activate();
        defaultShader->setMat4("MODEL_MATRIX", modelMatrix);
        defaultShader->setFloat("USE_TEXTURE", 0);
        defaultShader->setVec4("COLOR", Vec4f((float) color.r() / 255,
                                              (float) color.g() / 255,
                                              (float) color.b() / 255,
                                              (float) color.a() / 255));

        RenderCommand command(*defaultShader, buffer);
        command.properties.enableDepthTest = false;
        command.properties.enableBlending = true;

        renderDevice.getRenderer().addCommand(command);
    }

    void Renderer2D::draw(Vec2f position,
                          const std::string &text,
                          ColorRGBA color,
                          std::map<char, Character> &characters,
                          std::map<char, std::unique_ptr<TextureBuffer>> &textures) {
        float x = position.x;
        float y = position.y;

        for (auto &c: text) {
            auto &character = characters.at(c);

            float xpos = (x + static_cast<float>(character.bearing.x));
            float ypos = (y - static_cast<float>(character.bearing.y));

            float w = static_cast<float>(character.image.getSize().x);
            float h = static_cast<float>(character.image.getSize().y);

            x += static_cast<float>(static_cast<float>(character.advance));

            Mesh mesh = getPlane(Vec2f(w, h), Vec2f(), Rectf(Vec2f(), Vec2f(w, h)));

            MeshBuffer &buffer = **allocatedMeshes.insert(renderDevice.getAllocator().createMeshBuffer(mesh)).first;

            Mat4f modelMatrix = MatrixMath::identity();
            modelMatrix = modelMatrix * MatrixMath::translate(Vec3f(xpos, ypos, 0));
            modelMatrix = camera.projection() * camera.view() * modelMatrix;

            defaultTextShader->activate();
            defaultTextShader->setMat4("MODEL_MATRIX", modelMatrix);
            defaultTextShader->setVec4("COLOR", Vec4f((float) color.r() / 255,
                                                      (float) color.g() / 255,
                                                      (float) color.b() / 255,
                                                      (float) color.a() / 255));
            defaultTextShader->setTexture("diffuse", 0);

            RenderCommand command(*defaultTextShader, buffer);
            command.textures.emplace_back(*textures.at(c));
            command.properties.enableDepthTest = false;
            command.properties.enableBlending = true;

            renderDevice.getRenderer().addCommand(command);
        }
    }

    void Renderer2D::renderPresent() {
        renderDevice.getRenderer().renderFinish();
        allocatedMeshes.clear();
    }
}