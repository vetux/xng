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

#ifndef XENGINE_RENDERFONT_HPP
#define XENGINE_RENDERFONT_HPP

#include "xng/font/fontrenderer.hpp"
#include "xng/renderer/renderallocator.hpp"

namespace xng {
    class RenderFont {
    public:
        struct RenderGlyph {
            Glyph::Metrics metrics{};
            RenderObjectHandle<RenderTexture> texture;
            bool grayscale{};
        };

        RenderFont(RenderAllocator &allocator,
                   std::vector<std::unique_ptr<FontRenderer> > _fonts,
                   const Vec2i &pixelSize)
            : allocator(allocator),
              fonts(std::move(_fonts)) {
            for (const auto &font: fonts) {
                font->setPixelSize(pixelSize);
            }
        }

        ~RenderFont() = default;

        void loadGlyph(const char32_t c) {
            if (glyphs.find(c) != glyphs.end()) {
                return;
            }
            for (const auto &font: fonts) {
                if (font->check(c)) {
                    loadGlyph(font->render(c));
                    return;
                }
            }
        }

        FontMetrics getMetrics() const {
            return fonts.at(0)->getFontMetrics();
        }

        const std::unordered_map<char32_t, Glyph::Metrics> &getGlyphMetrics() const {
            return glyphMetrics;
        }

        RenderGlyph getGlyph(const char32_t c) {
            const auto it = glyphs.find(c);
            if (it != glyphs.end()) {
                return it->second;
            }
            loadGlyph(c);
            return glyphs.at(c);
        }

    private:
        void loadGlyph(const Glyph &glyph) {
            RenderGlyph g;

            if (glyph.metrics.bitmapSize.x > 0 && glyph.metrics.bitmapSize.y > 0) {
                ImageRGBA image;
                if (glyph.bitmap.index() == 0) {
                    const auto &imageGS = std::get<ImageGrayscale>(glyph.bitmap);
                    image = ImageRGBA(imageGS.getWidth(), imageGS.getHeight());
                    for (unsigned int y = 0; y < image.getHeight(); ++y) {
                        for (unsigned int x = 0; x < image.getWidth(); ++x) {
                            image.setPixel(x, y, ColorRGBA(imageGS.getPixel(x, y), imageGS.getPixel(x, y),
                                                           imageGS.getPixel(x, y), imageGS.getPixel(x, y)));
                        }
                    }
                    g.grayscale = true;
                } else {
                    image = std::get<ImageRGBA>(glyph.bitmap);
                    g.grayscale = false;
                }

                g.texture = allocator.createTexture(image, WRAP_CLAMP_TO_EDGE, 1);
            }

            g.metrics = glyph.metrics;

            glyphs[glyph.character] = g;
            glyphMetrics[glyph.character] = glyph.metrics;
        }

        RenderAllocator &allocator;

        std::vector<std::unique_ptr<FontRenderer> > fonts;

        std::unordered_map<char32_t, RenderGlyph> glyphs;
        std::unordered_map<char32_t, Glyph::Metrics> glyphMetrics;
    };
}
#endif //XENGINE_RENDERFONT_HPP
