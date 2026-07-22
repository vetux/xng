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

#ifndef XENGINE_RENDERTEXT_HPP
#define XENGINE_RENDERTEXT_HPP

#include "xng/renderer/renderfont.hpp"

#include "xng/layout/text/textlayoutengine.hpp"

namespace xng {
    class RenderText {
    public:
        RenderText(RenderScene &scene,
                   const RenderObjectHandle<RenderCanvas> &canvas,
                   std::shared_ptr<RenderFont> _font,
                   const std::u32string &text,
                   const TextLayoutParameters &layoutParameters,
                   const ColorRGBA &color,
                   const SamplingProperties &sampling_properties,
                   const Vec2f &position = {})
            : font(std::move(_font)),
              color(color),
              samplingProperties(sampling_properties) {
            if (text.empty()) {
                return;
            }

            for (auto &c: text) {
                font->loadGlyph(c);
            }
            layout = TextLayoutEngine::getLayout(text,
                                                 layoutParameters,
                                                 font->getMetrics(),
                                                 font->getGlyphMetrics());
            for (auto &c: layout.characters) {
                const auto &g = font->getGlyph(c.character);

                //TODO: Fix unit quad uv / positions

                // The unit quad is a device coordinate normalized quad because it is also used by fullscreen passes
                // The uv should already be correct in the mesh definition, so this y flip should not be necessary.

                // Map centered [-1,1] quad (Y-up) onto a corner-anchored box of size bitmapSize (Y-down screen space)
                const auto halfSize = Vec2f(static_cast<float>(g.metrics.bitmapSize.x) * 0.5f,
                                            static_cast<float>(g.metrics.bitmapSize.y) * 0.5f);

                Rectf dstRect;
                dstRect.position = position + c.position + halfSize;
                dstRect.dimensions = Vec2f(halfSize.x, -halfSize.y); // Flip Y (Y-up to Y-down)

                auto paint = scene.createPaint(canvas,
                                               dstRect,
                                               g.texture,
                                               {},
                                               color,
                                               Vec4f(1, 1, 1, 0));

                paint->flush();

                chars.emplace_back(paint);
            }
        }

        ~RenderText() = default;

        [[nodiscard]] const TextLayout &getLayout() const {
            return layout;
        }

        [[nodiscard]] const std::vector<RenderObjectHandle<RenderPaint> > &getChars() const {
            return chars;
        }

        [[nodiscard]] const ColorRGBA &getColor() const {
            return color;
        }

        [[nodiscard]] const SamplingProperties &getSamplingProperties() const {
            return samplingProperties;
        }

    private:
        std::shared_ptr<RenderFont> font;
        TextLayout layout{};

        ColorRGBA color;
        SamplingProperties samplingProperties;

        std::vector<RenderObjectHandle<RenderPaint> > chars;
    };
}

#endif //XENGINE_RENDERTEXT_HPP
