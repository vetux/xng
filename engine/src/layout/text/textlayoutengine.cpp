/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "xng/layout/text/textlayoutengine.hpp"

#include <utility>

namespace xng {
    Vec2i TextLayoutEngine::getSize(const std::u32string &str,
                                    const TextLayoutParameters &layout,
                                    const FontMetrics &fontMetrics,
                                    const std::unordered_map<char32_t, Glyph::Metrics> &glyphMetrics) {
        size_t numberOfLines = 1;
        size_t currentLineWidth = 0;
        size_t maximumLineWidth = 0;
        for (auto c: str) {
            if (c < 32) {
                // Non Printable Char
                if (c == '\n') {
                    numberOfLines++;
                    currentLineWidth = 0;
                }
                continue;
            }
            const auto metrics = glyphMetrics.at(c);
            if ((layout.maxLineWidth > 0 && currentLineWidth + metrics.advance > layout.maxLineWidth)) {
                numberOfLines++;
                currentLineWidth = 0;
            }
            currentLineWidth += metrics.advance;
            if (currentLineWidth > maximumLineWidth)
                maximumLineWidth = currentLineWidth;
        }

        return Vec2i(static_cast<int>(maximumLineWidth),
                     static_cast<int>((numberOfLines * (fontMetrics.height)) + (fontMetrics.descender * -1)));
    }

    int getWidth(const std::vector<TextLayout::Character> &line,
                 const std::unordered_map<char32_t, Glyph::Metrics> &glyphMetrics) {
        auto ret = 0;
        for (auto &c: line) {
            ret += glyphMetrics.at(c.character).advance;
        }
        return ret;
    }

    TextLayout TextLayoutEngine::getLayout(const std::u32string &text,
                                           const TextLayoutParameters &layoutParameters,
                                           const FontMetrics &fontMetrics,
                                           const std::unordered_map<char32_t, Glyph::Metrics> &glyphMetrics) {
        if (text.empty())
            throw std::runtime_error("Text cannot be empty");

        float posx = 0;
        int largestWidth = 0;

        auto lines = std::vector<std::vector<TextLayout::Character> >();
        lines.emplace_back();

        for (auto &c: text) {
            auto lineIndex = lines.size() - 1;

            if (c < 32) {
                // Non Printable Char
                if (c == '\n') {
                    lines.emplace_back();
                    posx = 0;
                    lineIndex = lines.size() - 1;
                }
                continue;
            }

            auto &character = glyphMetrics.at(c);
            auto lineWidth = getWidth(lines.at(lineIndex), glyphMetrics);

            if ((layoutParameters.maxLineWidth > 0 && lineWidth + character.advance > layoutParameters.maxLineWidth)) {
                lines.emplace_back();
                posx = 0;
                lineIndex = lines.size() - 1;
                lineWidth = 0;
            }

            if (lineWidth + character.advance > largestWidth)
                largestWidth = lineWidth + character.advance;

            float posy = (static_cast<float>(lineIndex) * static_cast<float>(layoutParameters.lineSpacing))
                         + (static_cast<float>(lineIndex) * static_cast<float>(fontMetrics.height));

            TextLayout::Character renderChar(Vec2f(posx, posy), c);

            // Add horizontal advance
            posx += static_cast<float>(character.advance);

            lines.at(lineIndex).emplace_back(renderChar);
        }

        auto origin = Vec2f(0, static_cast<float>(fontMetrics.ascender + fontMetrics.descender));

        // Apply alignment offset
        std::vector<TextLayout::Character> renderText;
        for (const auto &line: lines) {
            auto width = getWidth(line, glyphMetrics);
            float diff = static_cast<float>(largestWidth) - static_cast<float>(width);

            float offset = 0;
            switch (layoutParameters.alignment) {
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
                const auto &metrics = glyphMetrics.at(c.character);
                c.position.x += offset;
                c.position.x = c.position.x + (origin.x + static_cast<float>(metrics.bearing.x));
                c.position.y = c.position.y + (origin.y - static_cast<float>(metrics.bearing.y));
                renderText.emplace_back(c);
            }
        }
        return {getSize(text, layoutParameters, fontMetrics, glyphMetrics), renderText};
    }
}
