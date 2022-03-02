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

#include "text/textrenderer.hpp"

#include "render/shader/shaderinclude.hpp"

static const char *const SHADER_VERT = R"###("
#version 410 core

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

void main()
{
    fPosition = MVP * vec4(position, 1);
    fUv = uv;
    gl_Position = fPosition;
}
)###";

static const char *const SHADER_FRAG = R"###("
#version 410 core

layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;

layout (location = 0) out vec4 color;

uniform mat4 MVP;

uniform sampler2D diffuse;

void main() {
    color = vec4(texture(diffuse, fUv).r, 0, 0, 0);
}
)###";

namespace xengine {
    struct RenderChar {
        std::reference_wrapper<Character> character;
        std::reference_wrapper<TextureBuffer> texture; // The character texture
        std::reference_wrapper<MeshBuffer> mesh; // The meshbuffer scaled in the x and y plane according to the character dimensions

        Vec2f totalAdvance; // The total advance in the text of the character

        RenderChar(Character &character, TextureBuffer &texture, MeshBuffer &mesh)
                : character(character), texture(texture), mesh(mesh) {}

        /**
         * @param z The z value
         * @return The model matrix to apply including position, advance and bearing in a text string.
         */
        Vec2f getPosition() const {
            return {numeric_cast<float>(character.get().bearing.x) + totalAdvance.x,
                    numeric_cast<float>(character.get().bearing.y) + totalAdvance.y};
        }
    };

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

    TextRenderer::TextRenderer(Font &font, RenderDevice &device)
            : ascii(font.renderAscii()), device(&device), ren2d(device) {
        ShaderSource vs(SHADER_VERT, "main", VERTEX, GLSL_410);
        ShaderSource fs(SHADER_FRAG, "main", FRAGMENT, GLSL_410);

        vs.preprocess(ShaderInclude::getShaderIncludeCallback(),
                      ShaderInclude::getShaderMacros(GLSL_410));
        fs.preprocess(ShaderInclude::getShaderIncludeCallback(),
                      ShaderInclude::getShaderMacros(GLSL_410));

        shader = device.getAllocator().createShaderProgram(vs, fs);
        shader->setTexture("diffuse", 0);

        for (auto &c: ascii) {
            auto &character = c.second;

            float w = static_cast<float>(character.image.getSize().x);
            float h = static_cast<float>(character.image.getSize().y);

            Mesh mesh = getPlane(Vec2f(w, h), Vec2f(), Rectf(Vec2f(), Vec2f(w, h)));

            meshes[c.first] = device.getAllocator().createMeshBuffer(mesh);
            textures[c.first] = device.getAllocator().createTextureBuffer({});

            textures.at(c.first)->upload(character.image);
        }
    }

    Vec2f TextRenderer::getSize(const std::string &str, int maxCharPerLine) {
        Vec2i size(0); //The total size of the text

        Vec2i lineSize(0); // The size of the line and column of the current character

        int line = 0; //The index of the current line
        int lineOffset = 0; //The index into the current line

        for (auto c: str) {
            if (maxCharPerLine > 0 && lineOffset > maxCharPerLine) {
                line++;
                lineOffset = 0;
                lineSize.x = 0;
            }

            //Add advance (The only factor for size x increment)
            lineSize.x += ascii.at(c).advance.x;

            // Add vertical advance of all top characters of the current column
            lineSize.y = 0;
            for (size_t i = lineOffset;
                 i < str.size();
                 i + maxCharPerLine > str.size()
                 ? i = str.size() - i
                 : i += maxCharPerLine) {
                lineSize.y += ascii.at(str.at(i)).advance.y;
            }

            //Assign current horizontal size of the line if it is larger than the current size
            if (lineSize.x > size.x) {
                size.x = lineSize.x;
            }

            // Assign current vertical size of the column if it is larger than the current size
            if (lineSize.y > size.y) {
                size.y = lineSize.y;
            }

            lineOffset++;
        }

        return size;
    }

    Text TextRenderer::render(const std::string &text, int maxCharPerLine) {
        if (!device)
            throw std::runtime_error("Device not assigned");

        auto size = getSize(text, maxCharPerLine);

        std::vector<RenderChar> renderText;
        float advanceX = 0;
        int line = 0;
        size_t lineChars = 0;
        for (auto &c: text) {
            if (maxCharPerLine > 0 && lineChars > maxCharPerLine) {
                //Increment line and reset lineChars and advanceX
                line++;
                lineChars = 0;
                advanceX = 0;
            }

            auto &character = ascii.at(c);

            RenderChar renderChar(ascii.at(c), *textures.at(c), *meshes.at(c));

            float advanceY;

            if (line > 0) {
                // Use total advance value of top character
                auto lineOffset = (line - 1) * maxCharPerLine;
                auto &topChar = renderText.at(lineOffset + lineChars);
                advanceY = numeric_cast<float>(topChar.totalAdvance.y);
            } else {
                advanceY = 0;
            }

            renderChar.totalAdvance.x = advanceX;
            renderChar.totalAdvance.y = advanceY;

            // Add horizontal advance
            advanceX += numeric_cast<float>(character.advance.x);

            renderText.emplace_back(renderChar);

            lineChars++;
        }

        auto tex = device->getAllocator().createTextureBuffer({size});

        target = device->getAllocator().createRenderTarget(size);

        // Render the text to a texture
        target->attachColor(0, *tex);

        ren2d.renderBegin(*target);

        for (auto &c: renderText) {
            auto texSize = c.texture.get().getAttributes().size.convert<float>();
            ren2d.draw(Rectf({}, texSize),
                       Rectf(c.getPosition(), texSize),
                       c.texture);
        }

        ren2d.renderPresent();

        target->detachColor(0);

        return {text, maxCharPerLine, std::move(tex)};
    }
}