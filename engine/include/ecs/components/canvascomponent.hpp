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

#ifndef XENGINE_CANVASCOMPONENT_HPP
#define XENGINE_CANVASCOMPONENT_HPP

#include "io/messageable.hpp"

namespace xng {
    /*
     * A canvas renders child canvas transform components to the screen using Renderer2D
     */
    struct XENGINE_EXPORT CanvasComponent : public Messageable {
        bool enabled = true;
        Vec2f cameraPosition;

        Vec2i viewportSize;
        Vec2i viewportOffset;

        Vec2f projectionSize; // If magnitude larger than 0 the size of the projection when rendering canvas elements, otherwise screenSize is used as viewport size
        bool lockAspectRatio; // If true the viewport is adjusted so that its dimensions match projectionSize and its position is centered on the screen
        bool fitToScreen; // If lockAspectRatio is enabled and fitToScreen is enabled the viewport is stretched to fit the screen while preserving aspect ratio of projectionSize if lockAspectRatio is set
        bool lockViewport; // If true the viewport is not updated by the canvas render system to allow the user to implement custom viewport handling

        bool clear = false;
        ColorRGBA clearColor = ColorRGBA::black();
        int layer; // The sorting layer of this canvas

        Messageable &operator<<(const Message &message) override {
            enabled = message.value("enabled", true);
            cameraPosition << message.value("cameraPosition");
            projectionSize << message.value("projectionSize");
            viewportSize << message.value("viewportSize");
            viewportOffset << message.value("viewportOffset");
            lockAspectRatio = message.value("lockAspectRatio", false);
            fitToScreen = message.value("fitToScreen", false);
            lockViewport = message.value("lockViewport", false);
            clear = message.value("clear", false);
            clearColor << message.value("clearColor");
            layer = message.value("layer", 0);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["enabled"] = enabled;
            cameraPosition >> message["cameraPosition"];
            projectionSize >> message["projectionSize"];
            viewportSize >> message["viewportSize"];
            viewportOffset >> message["viewportOffset"];
            message["lockAspectRatio"] = lockAspectRatio;
            message["fitToScreen"] = fitToScreen;
            message["lockViewport"] = lockViewport;
            message["clear"] = clear;
            clearColor >> message["clearColor"];
            message["layer"] = layer;
            return message;
        }

        float getViewportScale(const Vec2i &screenSize) const {
            if (projectionSize.magnitude() == 0)
                return 1;
            float scale;
            if (screenSize.x > screenSize.y) {
                scale = screenSize.y / projectionSize.y;
                if (scale > screenSize.x / projectionSize.x) {
                    scale = screenSize.x / projectionSize.x;
                }
            } else {
                scale = screenSize.x / projectionSize.x;
                if (scale > screenSize.y / projectionSize.y) {
                    scale = screenSize.y / projectionSize.y;
                }
            }
            return scale;
        }

        Vec2i getViewportSize(const Vec2i &screenSize) const {
            if (projectionSize.magnitude() > 0) {
                if (fitToScreen) {
                    float scale = getViewportScale(screenSize);
                    Vec2f ret = projectionSize * scale;
                    if (ret.x <= 0) {
                        ret.x = 1;
                    }
                    if (ret.y <= 0) {
                        ret.y = 1;
                    }
                    return ret.convert<int>();
                } else {
                    return projectionSize.convert<int>();
                }
            } else {
                return screenSize;
            }
        }

        Vec2i getViewportOffset(const Vec2i &screenSize) const {
            return (screenSize - getViewportSize(screenSize)) / 2;
        }

        /**
         * Called by the canvas render system before drawing the canvas transform
         *
         * @param screenSize
         */
        void updateViewport(const Vec2i &screenSize) {
            if (!lockViewport) {
                viewportOffset = getViewportOffset(screenSize);
                viewportSize = getViewportSize(screenSize);
            }
        }
    };
}

#endif //XENGINE_CANVASCOMPONENT_HPP
