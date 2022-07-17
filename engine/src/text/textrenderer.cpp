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

#include "text/textrenderer.hpp"

namespace xng {
    struct RenderChar {
        std::reference_wrapper<Character> character;
        std::reference_wrapper<TextureBuffer> texture; // The character texture

        Vec2f position; // The position of the origin of the character x = Sum of all horizontal advance values before it, y = Sum of all line heights and spacings above it

        RenderChar(Character &character, TextureBuffer &texture)
                : character(character), texture(texture) {}

        /**
         * @param z The z value
         * @return The model matrix to apply including position, advance and bearing in a text string.
         */
        Vec2f getPosition(Vec2f origin) const {
            return {position.x + (origin.x + numeric_cast<float>(character.get().bearing.x)),
                    (position.y + (origin.y - numeric_cast<float>(character.get().bearing.y)))};
        }
    };

    TextRenderer::TextRenderer(Font &font,
                               RenderDevice &device,
                               SPIRVCompiler &spirvCompiler,
                               SPIRVDecompiler &spirvDecompiler)
            : ascii(font.renderAscii()), device(device), font(font), ren2d(device, spirvCompiler, spirvDecompiler) {
        ascii = font.renderAscii();
        for (auto &c: ascii) {
            auto &character = c.second;

            textures[c.first] = device.createTextureBuffer({.size = character.image.getSize()});
            textures.at(c.first)->upload(RGBA,
                                         reinterpret_cast<const uint8_t *>(character.image.getData()),
                                         sizeof(ColorRGBA) * (character.image.getSize().x *
                                                              character.image.getSize().y));
        }
    }

    void TextRenderer::setFontSize(Vec2i pixelSize) {
        if (fontSize != pixelSize) {
            fontSize = pixelSize;
            font.setPixelSize(pixelSize);
            ascii = font.renderAscii();
            for (auto &c: ascii) {
                auto &character = c.second;

                textures[c.first] = device.createTextureBuffer({.size = character.image.getSize()});
                textures.at(c.first)->upload(RGBA,
                                             reinterpret_cast<const uint8_t *>(character.image.getData()),
                                             sizeof(ColorRGBA) * (character.image.getSize().x *
                                                                  character.image.getSize().y));
            }
        }
    }

    Vec2f TextRenderer::getSize(const std::string &str, int lineHeight, int lineWidth, int lineSpacing) {
        Vec2i size(0); //The total size of the text

        Vec2i lineSize(0); // The size of the line and column of the current character
        int line = 0; //The index of the current line

        for (auto c: str) {
            //Add horizontal advance
            auto character = ascii.at(c);
            lineSize.x += character.advance;

            lineSize.y = (line * lineSpacing) + (line * lineHeight) +
                         (lineHeight + character.image.getHeight() - character.bearing.y);

            //Assign current horizontal size of the line if it is larger than the current size
            if (lineSize.x > size.x) {
                size.x = lineSize.x;
            }

            // Assign current vertical size of the column if it is larger than the current size
            if (lineSize.y > size.y) {
                size.y = lineSize.y;
            }

            if (c == '\n' || (lineWidth > 0 && lineSize.x > lineWidth)) {
                line++;
                lineSize.x = 0;
            }
        }

        return size.convert<float>();
    }

    Text TextRenderer::render(const std::string &text,
                              int lineHeight,
                              int lineWidth,
                              int lineSpacing) {
        if (text.empty())
            throw std::runtime_error("Text cannot be empty");

        auto size = getSize(text, lineHeight, lineWidth, lineSpacing);

        std::vector<RenderChar> renderText;

        Character largestCharacterOfFirstLine;

        float posx = 0;

        int currentLineWidth = 0;
        int line = 0;

        for (auto &c: text) {
            auto &character = ascii.at(c);

            currentLineWidth += character.advance;

            if (c == '\n' || (lineWidth > 0 && currentLineWidth > lineWidth)) {
                line++;
                posx = 0;
                currentLineWidth = 0;
            }

            if (c < 32)
                continue; // Skip non printable characters

            RenderChar renderChar(character, *textures.at(c));

            float posy = (numeric_cast<float>(line) * numeric_cast<float>(lineSpacing))
                         + (numeric_cast<float>(line) * numeric_cast<float>(lineHeight));

            renderChar.position.x = posx;
            renderChar.position.y = posy;

            // Add horizontal advance
            posx += numeric_cast<float>(character.advance);

            if (line == 0 && largestCharacterOfFirstLine.image.getHeight() < character.image.getHeight())
                largestCharacterOfFirstLine = character;

            renderText.emplace_back(renderChar);
        }

        auto origin = Vec2f(0, numeric_cast<float>(lineHeight));

        target = device.createRenderTarget({.size = size.convert<int>()});

        // Render the text (upside down?) to a texture and then render the final text texture using the 2d renderer
        auto tmpTexture = device.createTextureBuffer({.size = size.convert<int>()});
        target->setColorAttachments({tmpTexture.get()});
        ren2d.renderBegin(*target);
        for (auto &c: renderText) {
            auto texSize = c.texture.get().getDescription().size.convert<float>();
            auto pos = c.getPosition(origin);
            ren2d.draw(Rectf({}, texSize),
                       Rectf(pos, texSize),
                       c.texture,
                       {},
                       0,
                       Vec2b(false, false));
        }
        ren2d.renderPresent();

        target->setColorAttachments(std::vector<TextureBuffer *>());

        //Render the upside down texture of the text using the 2d renderer to correct the rotation?
        auto tex = device.createTextureBuffer({size.convert<int>()});

        target->setColorAttachments({tex.get()});
        ren2d.renderBegin(*target);
        ren2d.draw(Rectf({}, size), Rectf({}, size), *tmpTexture, {}, 0, {false, false});
        ren2d.renderPresent();
        target->setColorAttachments(std::vector<TextureBuffer *>());

        return {text, origin, lineWidth, std::move(tex)};
    }
}