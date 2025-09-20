/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include "xng/graphics/2d/text/textrenderer.hpp"

#include <utility>

namespace xng {
    struct RenderChar {
        std::reference_wrapper<Character> character;
        Texture2D texture; // The character texture

        // The position of the origin of the character x = Sum of all horizontal advance values before it, y = Sum of all line heights and spacings above it
        Vec2f position;

        RenderChar(Character &character, Texture2D texture)
            : character(character), texture(std::move(texture)) {
        }

        Vec2f getPosition(const Vec2f &origin) const {
            return {
                position.x + (origin.x + static_cast<float>(character.get().bearing.x)),
                (position.y + (origin.y - static_cast<float>(character.get().bearing.y)))
            };
        }
    };

    TextRenderer::TextRenderer(std::shared_ptr<Renderer2D> ren2D,
                               FontRenderer &font,
                               const Vec2i &pixelSize)
        : ren2d(std::move(ren2D)), pixelSize(pixelSize) {
        font.setPixelSize(pixelSize);
        ascii = font.renderAscii();
        for (auto &c: ascii) {
            textures[c.first] = ren2d->createTexture(c.second.image);
        }
    }

    TextRenderer::~TextRenderer() {
        for (auto &tex: textures) {
            ren2d->destroyTexture(tex.second);
        }
        textures.clear();
    }

    Vec2i TextRenderer::getSize(const std::string &str, const TextLayout &layout) const {
        Vec2i size(0); //The total size of the text

        Vec2i lineSize(0); // The size of the line and column of the current character
        int line = 0; //The index of the current line

        for (auto c: str) {
            //Add horizontal advance
            auto character = ascii.at(c);
            if (c == '\n' || (layout.lineWidth > 0 && lineSize.x + character.advance > layout.lineWidth)) {
                line++;
                lineSize.x = 0;
            }

            lineSize.x += character.advance;

            lineSize.y = ((line + 1) * layout.lineSpacing)
                         + ((line + 1) * layout.lineHeight);

            //Assign the current horizontal size of the line if it is larger than the current size
            if (lineSize.x > size.x) {
                size.x = lineSize.x;
            }

            // Assign the current vertical size of the column if it is larger than the current size
            if (lineSize.y > size.y) {
                size.y = lineSize.y;
            }
        }

        return size;
    }

    int getWidth(const std::vector<RenderChar> &line) {
        auto ret = 0;
        for (auto &c: line) {
            ret += c.character.get().advance;
        }
        return ret;
    }

    Text TextRenderer::render(const std::string &text, const TextLayout &layout) {
        if (text.empty())
            throw std::runtime_error("Text cannot be empty");

        auto size = getSize(text, layout);

        Character largestCharacterOfFirstLine;

        float posx = 0;

        int largestWidth = 0;
        std::vector<std::vector<RenderChar> > lines = std::vector<std::vector<RenderChar> >();
        lines.emplace_back();

        for (auto &c: text) {
            auto lineIndex = lines.size() - 1;
            auto &character = ascii.at(c);
            auto lineWidth = getWidth(lines.at(lineIndex));

            if (c == '\n'
                || (layout.lineWidth > 0 && lineWidth + character.advance > layout.lineWidth)) {
                lines.emplace_back();
                posx = 0;
                lineIndex = lines.size() - 1;
                lineWidth = 0;
            }

            if (c < 32)
                continue; // Skip non printable characters

            if (lineWidth + character.advance > largestWidth)
                largestWidth = lineWidth + character.advance;

            RenderChar renderChar(character, textures.at(c));

            float posy = (static_cast<float>(lineIndex) * static_cast<float>(layout.lineSpacing))
                         + (static_cast<float>(lineIndex) * static_cast<float>(layout.lineHeight));

            renderChar.position.x = posx;
            renderChar.position.y = posy;

            // Add horizontal advance
            posx += static_cast<float>(character.advance);

            if (lines.size() == 1 && largestCharacterOfFirstLine.image.getHeight() < character.image.getHeight())
                largestCharacterOfFirstLine = character;

            lines.at(lineIndex).emplace_back(renderChar);
        }

        // Apply alignment offset
        std::vector<RenderChar> renderText;
        for (const auto &line: lines) {
            auto width = getWidth(line);
            float diff = static_cast<float>(largestWidth) - static_cast<float>(width);

            float offset = 0;
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

        auto origin = Vec2f(0, static_cast<float>(layout.lineHeight));

        auto target = ren2d->createTexture(ImageRGBA(size));

        auto sizef = size.convert<float>();
        ren2d->renderBegin(target,
                           true,
                           ColorRGBA::white(1, 0),
                           {},
                           size,
                           {},
                           Rectf({0, sizef.y}, {sizef.x, 0}));

        for (auto &c: renderText) {
            auto texSize = c.texture.getSize().convert<float>();
            auto pos = c.getPosition(origin);
            ren2d->draw(Rectf({}, texSize),
                        Rectf(pos, texSize),
                        c.texture,
                        {},
                        0,
                        NEAREST,
                        1,
                        0,
                        ColorRGBA::black());
        }

        ren2d->renderPresent();

        return {text, origin, layout, target, ren2d};
    }
}
