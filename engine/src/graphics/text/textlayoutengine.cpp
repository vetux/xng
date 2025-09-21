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

#include "xng/graphics/text/textlayoutengine.hpp"

#include <utility>

namespace xng {
    TextLayoutEngine::TextLayoutEngine(FontEngine &fontEngine,
                                       const ResourceHandle<Font> &font,
                                       const Vec2i &fontPixelSize)
        : fontUri(font.getUri()), fontPixelSize(fontPixelSize) {
        const auto fontRenderer = fontEngine.createFontRenderer(font.get());
        fontRenderer->setPixelSize(fontPixelSize);
        ascender = fontRenderer->getAscender();
        descender = fontRenderer->getDescender();
        lineHeight = fontRenderer->getHeight();
        ascii = fontRenderer->renderAscii();
    }

    Vec2i TextLayoutEngine::getSize(const std::string &str, const TextLayoutParameters &layout) const {
        size_t numberOfLines = 1;
        size_t currentLineWidth = 0;
        size_t maximumLineWidth = 0;
        for (auto c: str) {
            auto &chr = ascii.at(c);
            if (c == '\n' || (layout.maxLineWidth > 0 && currentLineWidth + chr.advance > layout.maxLineWidth)) {
                numberOfLines++;
                currentLineWidth = 0;
            }
            currentLineWidth += chr.advance;
            if (currentLineWidth > maximumLineWidth)
                maximumLineWidth = currentLineWidth;
        }

        return Vec2i(maximumLineWidth, (numberOfLines * (lineHeight)) + (descender * -1));
    }

    int getWidth(const std::vector<TextLayout::Glyph> &line) {
        auto ret = 0;
        for (auto &c: line) {
            ret += c.character.get().advance;
        }
        return ret;
    }

    TextLayout TextLayoutEngine::getLayout(const std::string &text, const TextLayoutParameters &layout) const {
        if (text.empty())
            throw std::runtime_error("Text cannot be empty");

        float posx = 0;

        int largestWidth = 0;
        std::vector<std::vector<TextLayout::Glyph> > lines = std::vector<std::vector<TextLayout::Glyph> >();
        lines.emplace_back();

        for (auto &c: text) {
            auto lineIndex = lines.size() - 1;
            auto &character = ascii.at(c);
            auto lineWidth = getWidth(lines.at(lineIndex));

            if (c == '\n'
                || (layout.maxLineWidth > 0 && lineWidth + character.advance > layout.maxLineWidth)) {
                lines.emplace_back();
                posx = 0;
                lineIndex = lines.size() - 1;
                lineWidth = 0;
            }

            if (c < 32)
                continue; // Skip non printable characters

            if (lineWidth + character.advance > largestWidth)
                largestWidth = lineWidth + character.advance;

            float posy = (static_cast<float>(lineIndex) * static_cast<float>(layout.lineSpacing))
                         + (static_cast<float>(lineIndex) * static_cast<float>(lineHeight));

            TextLayout::Glyph renderChar(Vec2f(posx, posy), character);

            // Add horizontal advance
            posx += static_cast<float>(character.advance);

            lines.at(lineIndex).emplace_back(renderChar);
        }

        auto origin = Vec2f(0, static_cast<float>(ascender + descender));

        // Apply alignment offset
        std::vector<TextLayout::Glyph> renderText;
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
                c.position.x = c.position.x + (origin.x + static_cast<float>(c.character.get().bearing.x));
                c.position.y = c.position.y + (origin.y - static_cast<float>(c.character.get().bearing.y));
                renderText.emplace_back(c);
            }
        }

        return {getSize(text, layout), renderText, fontUri, fontPixelSize, text, layout};
    }
}
