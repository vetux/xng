/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_DEBUGOVERLAY_HPP
#define XENGINE_DEBUGOVERLAY_HPP

#include "xng/xng.hpp"

using namespace xng;

class DebugOverlay {
public:
    Vec2i pixelSize = {0, 25};
    Renderer2D &ren2D;
    TextRenderer textRenderer;

    Text dynText;
    TextureAtlasHandle dynHandle;

    Text staticText;
    TextureAtlasHandle staticHandle;
    bool staticAlloc = false;

    DebugOverlay(Font &font, Renderer2D &renderer2D) : ren2D(renderer2D),
                                                       textRenderer(font, renderer2D, pixelSize) {}

    void draw(DeltaTime deltaTime,
              RenderTarget &screen,
              const std::string& appendText = {}) {
        if (!staticAlloc) {
            staticAlloc = true;
            std::string staticTxt;
            staticTxt += ren2D.getDevice().getInfo().vendor + " " + ren2D.getDevice().getInfo().renderer + "\n";
            staticTxt += ren2D.getDevice().getInfo().version;

            TextLayout layout;
            layout.lineHeight = pixelSize.y;

            staticText = textRenderer.render(staticTxt, layout);
            staticHandle = ren2D.createTexture(staticText.getImage());
        }

        auto deltaMs = std::to_string(deltaTime * 1000);
        auto fps = std::to_string(1 / deltaTime);

        std::stringstream strm;
        strm << std::fixed << std::setprecision(3) << deltaMs;

        std::string dynTxt;
        dynTxt += strm.str() + " / ";
        strm = {};

        strm << std::fixed << std::setprecision(0) << fps;

        dynTxt += strm.str() + "\n";

        auto stats = ren2D.getDevice().getRenderCommandQueues().at(0).get().debugNewFrame();

        dynTxt += "Draws:" + std::to_string(stats.drawCalls) + "\n";
        dynTxt += "Polys:" + std::to_string(stats.polys) + "\n";
        dynTxt += "Binds:" + std::to_string(stats.binds) + "\n";
        dynTxt += appendText;

        TextLayout layout;
        layout.lineHeight = pixelSize.y;

        dynText = textRenderer.render(dynTxt, layout);

        dynHandle = ren2D.createTexture(dynText.getImage());

        ren2D.renderBegin(screen, false, {}, {}, screen.getDescription().size, {});
        ren2D.draw(Rectf({}, dynText.getImage().getSize().convert<float>()),
                   Rectf({}, dynText.getImage().getSize().convert<float>()),
                   dynHandle,
                   {},
                   0,
                   xng::NEAREST,
                   ColorRGBA::yellow());

        ren2D.draw(Rectf({}, staticText.getImage().getSize().convert<float>()),
                   Rectf({0, static_cast<float>(screen.getDescription().size.y - staticText.getImage().getSize().y)}, staticText.getImage().getSize().convert<float>()),
                   staticHandle,
                   {},
                   0,
                   xng::NEAREST,
                   ColorRGBA::yellow());
        ren2D.renderPresent();

        ren2D.destroyTexture(dynHandle);
    }
};

#endif //XENGINE_DEBUGOVERLAY_HPP
