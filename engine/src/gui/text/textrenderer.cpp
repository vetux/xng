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

#include "xng/gui/text/textrenderer.hpp"

#pragma message "Not Implemented"

/*
namespace xng {
    struct RenderChar {
        std::reference_wrapper<Character> character;
        std::reference_wrapper<TextureBuffer> texture; // The character texture

        Vec2f position; // The position of the origin of the character x = Sum of all horizontal advance values before it, y = Sum of all line heights and spacings above it

        RenderChar(Character &character, TextureBuffer &texture)
                : character(character), texture(texture) {}

        Vec2f getPosition(Vec2f origin) const {
            return {position.x + (origin.x + numeric_cast<float>(character.get().bearing.x)),
                    (position.y + (origin.y - numeric_cast<float>(character.get().bearing.y)))};
        }
    };

    TextRenderer::TextRenderer(Font &font,
                               Renderer2D &ren2D)
            : ascii(font.renderAscii()), font(font), ren2d(ren2D) {
        ascii = font.renderAscii();
        for (auto &c: ascii) {
            auto &character = c.second;

            TextureBufferDesc desc;
            desc.size = character.image.getSize();

            textures[c.first] = ren2D.getDevice().createTextureBuffer(desc);
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

                TextureBufferDesc desc;
                desc.size = character.image.getSize();

                textures[c.first] = ren2d.getDevice().createTextureBuffer(desc);
                textures.at(c.first)->upload(RGBA,
                                             reinterpret_cast<const uint8_t *>(character.image.getData()),
                                             sizeof(ColorRGBA) * (character.image.getSize().x *
                                                                  character.image.getSize().y));
            }
        }
    }

    Vec2f TextRenderer::getSize(const std::string &str, const TextLayout &layout) {
        Vec2i size(0); //The total size of the text

        Vec2i lineSize(0); // The size of the line and column of the current character
        int line = 0; //The index of the current line

        for (auto c: str) {
            //Add horizontal advance
            auto character = ascii.at(c);
            lineSize.x += character.advance;

            lineSize.y = (line * layout.lineSpacing) + (line * layout.lineHeight) +
                         (layout.lineHeight + character.image.getHeight() - character.bearing.y);

            //Assign current horizontal size of the line if it is larger than the current size
            if (lineSize.x > size.x) {
                size.x = lineSize.x;
            }

            // Assign current vertical size of the column if it is larger than the current size
            if (lineSize.y > size.y) {
                size.y = lineSize.y;
            }

            if (c == '\n' || (layout.lineWidth > 0 && lineSize.x > layout.lineWidth)) {
                line++;
                lineSize.x = 0;
            }
        }

        return size.convert<float>();
    }


    int getWidth(const std::vector<RenderChar> &line) {
        auto ret = 0;
        for (auto &c: line) {
            ret += c.character.get().advance;
        }
        return ret;
    }

    Text TextRenderer::render(const std::string &text, const TextLayout &layout, const ColorRGBA &color) {
        if (text.empty())
            throw std::runtime_error("Text cannot be empty");

        auto size = getSize(text, layout);

        Character largestCharacterOfFirstLine;

        float posx = 0;

        int largestWidth = 0;
        std::vector<std::vector<RenderChar>> lines = std::vector<std::vector<RenderChar>>();
        lines.emplace_back(std::vector<RenderChar>());

        for (auto &c: text) {
            auto lineIndex = lines.size() - 1;
            auto &character = ascii.at(c);
            auto lineWidth = getWidth(lines.at(lineIndex));

            if (c == '\n'
                || (layout.lineWidth > 0 && lineWidth + character.advance > layout.lineWidth)) {
                lines.emplace_back(std::vector<RenderChar>());
                posx = 0;
                lineIndex = lines.size() - 1;
                lineWidth = 0;
            }

            if (c < 32)
                continue; // Skip non printable characters

            if (lineWidth + character.advance > largestWidth)
                largestWidth = lineWidth + character.advance;

            RenderChar renderChar(character, *textures.at(c));

            float posy = (numeric_cast<float>(lineIndex) * numeric_cast<float>(layout.lineSpacing))
                         + (numeric_cast<float>(lineIndex) * numeric_cast<float>(layout.lineHeight));

            renderChar.position.x = posx;
            renderChar.position.y = posy;

            // Add horizontal advance
            posx += numeric_cast<float>(character.advance);

            if (lines.size() == 1 && largestCharacterOfFirstLine.image.getHeight() < character.image.getHeight())
                largestCharacterOfFirstLine = character;

            lines.at(lineIndex).emplace_back(renderChar);
        }

        // Apply alignment offset
        std::vector<RenderChar> renderText;
        for (auto line: lines) {
            auto width = getWidth(line);
            auto diff = largestWidth - width;

            int offset = 0;
            switch (layout.alignment) {
                default:
                case TEXT_ALIGN_LEFT:
                    break;
                case TEXT_ALIGN_CENTER:
                    offset = diff / 2;
                    break;
                case TEXT_ALIGN_RIGHT:
                    offset = diff;
                    break;
            }

            for (auto c: line) {
                c.position.x += offset;
                renderText.emplace_back(c);
            }
        }

        auto origin = Vec2f(0, numeric_cast<float>(layout.lineHeight));

        target = ren2d.getDevice().createRenderTarget({.size = size.convert<int>()});

        TextureBufferDesc desc;
        desc.size = size.convert<int>();

        // Render the text (upside down?) to a texture and then render the final text texture using the 2d renderer
        auto tmpTexture = ren2d.getDevice().createTextureBuffer(desc);
        target->setColorAttachments({*tmpTexture});
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

        target->setColorAttachments({});

        //Render the upside down texture of the text using the 2d renderer to correct the rotation?
        auto tex = ren2d.getDevice().createTextureBuffer(desc);

        target->setColorAttachments({*tex});
        ren2d.renderBegin(*target);
        ren2d.draw(Rectf({}, size), Rectf({}, size), *tmpTexture, {}, 0, {false, false});
        ren2d.renderPresent();
        target->setColorAttachments({});

        return {text, origin, layout.lineWidth, std::move(tex)};
    }
}*/