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
#include "render/shader/shaderinclude.hpp"
#include "render/2d/renderer2d.hpp"

#include "math/matrixmath.hpp"
#include "async/threadpool.hpp"
#include "platform/graphics/shadercompiler.hpp"
#include "platform/graphics/shadercompiler.hpp"

static const char *SHADER_VERT = R"###(
float4x4 MODEL_MATRIX;
float USE_TEXTURE;
float4 COLOR;

struct VS_INPUT
{
    float3 position : POSITION0;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 instanceRow0 : POSITION1;
    float4 instanceRow1 : POSITION2;
    float4 instanceRow2 : POSITION3;
    float4 instanceRow3 : POSITION4;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2  uv : TEXCOORD0;
};

VS_OUTPUT main(const VS_INPUT v)
{
    VS_OUTPUT ret;

    ret.position = mul(float4(v.position, 1), MODEL_MATRIX);
    ret.uv = v.uv;

    return ret;
}
)###";

static const char *SHADER_FRAG = R"###(
float4x4 MODEL_MATRIX;
float USE_TEXTURE;
float4 COLOR;

struct PS_INPUT {
    float2 uv: TEXCOORD0;
};

struct PS_OUTPUT {
     float4 pixel     :   SV_TARGET0;
};

Texture2D diffuse;

SamplerState samplerState_diffuse
{};

PS_OUTPUT main(PS_INPUT v) {
    PS_OUTPUT ret;
    if (USE_TEXTURE != 0)
        ret.pixel = diffuse.Sample(samplerState_diffuse, v.uv);
    else
        ret.pixel = COLOR;
    return ret;
}
)###";

static const char *SHADER_TEXT_FRAG = R"###(
float4x4 MODEL_MATRIX;
float USE_TEXTURE;
float4 COLOR;

struct PS_INPUT {
    float2 uv: TEXCOORD0;
};

struct PS_OUTPUT {
     float4 pixel     :   SV_TARGET0;
};

Texture2D diffuse;

SamplerState samplerState_diffuse
{};

PS_OUTPUT main(PS_INPUT v) {
    PS_OUTPUT ret;
    float grayscale = diffuse.Sample(samplerState_diffuse, v.uv).r;
    ret.pixel = COLOR * grayscale;
    return ret;
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
        vs = ShaderSource(SHADER_VERT, "main", VERTEX, HLSL_SHADER_MODEL_4);
        fs = ShaderSource(SHADER_FRAG, "main", FRAGMENT, HLSL_SHADER_MODEL_4);
        fsText = ShaderSource(SHADER_TEXT_FRAG, "main", FRAGMENT, HLSL_SHADER_MODEL_4);

        vs.preprocess(ShaderInclude::getShaderIncludeCallback(),
                      ShaderInclude::getShaderMacros(HLSL_SHADER_MODEL_4));
        fs.preprocess(ShaderInclude::getShaderIncludeCallback(),
                      ShaderInclude::getShaderMacros(HLSL_SHADER_MODEL_4));
        fsText.preprocess(ShaderInclude::getShaderIncludeCallback(),
                          ShaderInclude::getShaderMacros(HLSL_SHADER_MODEL_4));

        defaultShader = device.getAllocator().createShaderProgram(vs, fs);
        defaultTextShader = device.getAllocator().createShaderProgram(vs, fsText);
    }

    Renderer2D::~Renderer2D() = default;

    void Renderer2D::renderBegin(RenderTarget &target, bool clear) {
        renderDevice.getRenderer().renderBegin(target, RenderOptions({},
                                                                     target.getSize(),
                                                                     true,
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