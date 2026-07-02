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

#ifndef XENGINE_RENDERPAINTTEXT_HPP
#define XENGINE_RENDERPAINTTEXT_HPP

#include "xng/math/matrixmath.hpp"
#include "xng/layout/text/textlayout.hpp"
#include "xng/layout/text/textlayoutengine.hpp"
#include "xng/layout/text/textlayoutparameters.hpp"
#include "xng/renderer/objects/renderpaint.hpp"
#include "xng/renderer/objects/renderfont.hpp"

namespace xng {
    class RenderPaintText final : public RenderPaint {
    public:
        struct RenderChar {
            Mat4f modelMatrix; // For scaling / positioning a unit quad
            Vec2i textureSize{};
            VirtualTextureStreamer::TextureID texture{};
            bool grayscale{};
        };

        RenderPaintText(const Id id,
                        const RenderObjectHandle<RenderFont> &font,
                        const std::u32string &text,
                        const TextLayoutParameters &layoutParameters,
                        const ColorRGBA &color,
                        const SamplingProperties &sampling_properties)
            : RenderPaint(OBJECT_PAINT_TEXT, id),
              font(font),
              color(color),
              samplingProperties(sampling_properties) {
            for (auto &c: text) {
                font->loadGlyph(c);
            }
            layout = TextLayoutEngine::getLayout(text,
                                                 layoutParameters,
                                                 font->getMetrics(),
                                                 font->getGlyphMetrics());
            for (auto &c: layout.characters) {
                const auto &g = font->getGlyph(c.character);
                RenderChar rc{};
                // Map centered [-1,1] quad (Y-up) onto a corner-anchored box of size bitmapSize (Y-down screen space)
                const auto halfSize = Vec3f(static_cast<float>(g.metrics.bitmapSize.x) * 0.5f,
                                            static_cast<float>(g.metrics.bitmapSize.y) * 0.5f,
                                            1.0f);
                rc.modelMatrix = MatrixMath::translate(Vec3f(c.position.x + halfSize.x,
                                                             c.position.y + halfSize.y,
                                                             0))
                                 * MatrixMath::scale(Vec3f(halfSize.x, -halfSize.y, 1));
                rc.texture = g.texture;
                rc.grayscale = g.grayscale;
                rc.textureSize = g.metrics.bitmapSize;
                chars.emplace_back(rc);
            }
        }

        ~RenderPaintText() override {
        }

        [[nodiscard]] const TextLayout &getLayout() const {
            return layout;
        }

        [[nodiscard]] const std::vector<RenderChar> &getChars() const {
            return chars;
        }

        [[nodiscard]] const ColorRGBA &getColor() const {
            return color;
        }

        [[nodiscard]] const SamplingProperties &getSamplingProperties() const {
            return samplingProperties;
        }

        bool isUploadComplete() override {
            return true;
        }

        void flush() override {
        }

    private:
        RenderObjectHandle<RenderFont> font;
        TextLayout layout{};

        ColorRGBA color;
        SamplingProperties samplingProperties;

        std::vector<RenderChar> chars;
    };
}

#endif //XENGINE_RENDERPAINTTEXT_HPP
