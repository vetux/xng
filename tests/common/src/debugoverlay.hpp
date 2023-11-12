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

#include <list>

using namespace xng;

class DebugOverlay {
public:
    Vec2i fontSize = {0, 20};
    Renderer2D &ren2D;
    TextRenderer textRenderer;

    Text dynText;
    Texture2D dynTexture;

    Text staticText;
    Texture2D staticTexture;
    bool staticAlloc = false;

    float fpsAverageDuration = 0.5;
    std::list<float> fpsAverage;

    float getAverageFps(float currentFps) {
        fpsAverage.emplace_back(currentFps);

        int fpsCount = 0;
        float totalTime = 0;
        for (auto &val: fpsAverage) {
            if (val > 0) {
                fpsCount++;
            }
            totalTime += 1 / val;
        }

        while (totalTime > fpsAverageDuration && !fpsAverage.empty()) {
            fpsAverage.erase(fpsAverage.begin());
            fpsCount--;
            totalTime = 0;
            for (auto &val: fpsAverage) {
                totalTime += 1 / val;
            }
        }

        return static_cast<float>(fpsCount) / totalTime;
    }

    DebugOverlay(Font &font, Renderer2D &renderer2D) : ren2D(renderer2D),
                                                       textRenderer(font, renderer2D, fontSize) {}

    std::string formatBytes(size_t bytes) {
        if (bytes > 1000000000) {
            return std::to_string(bytes / 1000000000) + "GB";
        } else if (bytes > 1000000){
            return std::to_string(bytes / 1000000) + "MB";
        } else if (bytes > 1000){
            return std::to_string(bytes / 1000) + "KB";
        } else {
            return std::to_string(bytes) + "B";
        }
    }

    void draw(DeltaTime deltaTime,
              RenderTarget &screen,
              const std::string &appendText = {}) {
        if (!staticAlloc) {
            staticAlloc = true;
            std::string staticTxt;
            staticTxt += ren2D.getDevice().getInfo().vendor + " " + ren2D.getDevice().getInfo().renderer + "\n";
            staticTxt += ren2D.getDevice().getInfo().version;

            TextLayout layout;
            layout.lineHeight = fontSize.y;

            staticText = textRenderer.render(staticTxt, layout);
            staticTexture = ren2D.createTexture(staticText.getImage());
        }

        auto deltaMs = deltaTime * 1000;
        auto fps = 1 / deltaTime;
        auto averageFps = getAverageFps(static_cast<float>(fps));

        std::stringstream strm;
        strm << std::fixed << std::setprecision(3) << deltaMs;

        auto msStr = strm.str();

        if (msStr.size() < 7){
            for (auto i = msStr.size(); i < 7; i++)
                msStr = ' ' + msStr;
        }

        strm = {};

        strm << std::fixed << std::setprecision(0) << averageFps;

        auto fpsStr = strm.str();

        if (fpsStr.size() < 3){
            for (auto i = fpsStr.size(); i < 3; i++)
                fpsStr += ' ';
        }

        std::string dynTxt;
        dynTxt += fpsStr + " ";
        dynTxt += msStr + "\n";

        auto stats = ren2D.getDevice().getFrameStats();

        dynTxt += "Draws:" + std::to_string(stats.drawCalls) + "\n";
        dynTxt += "Polys:" + std::to_string(stats.polys) + "\n";
        dynTxt += "Binds:" + std::to_string(stats.binds) + "\n";
        dynTxt += "VRam Upload:" + formatBytes(stats.getTotalUpload()) + "\n";
        dynTxt += "VRam Download:" + formatBytes(stats.getTotalDownload()) + "\n";
        dynTxt += appendText;

        TextLayout layout;
        layout.lineHeight = fontSize.y;

        dynText = textRenderer.render(dynTxt, layout);

        dynTexture = ren2D.createTexture(dynText.getImage());

        ren2D.renderBegin(screen, false, {}, {}, screen.getDescription().size, {});
        ren2D.draw(Rectf({}, dynText.getImage().getSize().convert<float>()),
                   Rectf({}, dynText.getImage().getSize().convert<float>()),
                   dynTexture,
                   {},
                   0,
                   xng::NEAREST,
                   ColorRGBA::yellow());

        ren2D.draw(Rectf({}, staticText.getImage().getSize().convert<float>()),
                   Rectf({0, static_cast<float>(screen.getDescription().size.y - staticText.getImage().getSize().y)},
                         staticText.getImage().getSize().convert<float>()),
                   staticTexture,
                   {},
                   0,
                   xng::NEAREST,
                   ColorRGBA::yellow());
        ren2D.renderPresent();

        dynTexture = {};
    }
};

#endif //XENGINE_DEBUGOVERLAY_HPP
