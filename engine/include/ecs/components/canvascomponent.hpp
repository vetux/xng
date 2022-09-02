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
     * A canvas renders child canvas transform components to the screen using Renderer2D with the supplied cameraPosition and projection size
     */
    struct XENGINE_EXPORT CanvasComponent : public Messageable {
        bool enabled = true;
        Vec2f cameraPosition;

        Vec2f projectionSize; // If magnitude larger than 0 the size of the projection when rendering canvas elements

        Vec2i viewportSize;
        Vec2i viewportOffset;

        bool lockAspectRatio = false; // If true the viewport is adjusted so that the projection aspect ratio matches the projectionSize

        // If false the viewport size matches the projectionSize and the canvas is cut off if the window is smaller than projectionSize
        // If true the viewport size is stretched to the screenSize while the aspect ratio of projectionSize is preserved
        bool stretchProjection = false;

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
            stretchProjection = message.value("stretchProjection", false);
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
            message["stretchProjection"] = stretchProjection;
            message["clear"] = clear;
            clearColor >> message["clearColor"];
            message["layer"] = layer;
            return message;
        }

        float getScale(const Vec2f &screenSize) const {
            float scale = 1;
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

        Vec2f getSize(const Vec2f &screenSize) const {
            if (projectionSize.magnitude() > 0) {
                if (stretchProjection) {
                    float scale = getScale(screenSize);
                    Vec2f ret = projectionSize * scale;
                    if (ret.x <= 0) {
                        ret.x = 1;
                    }
                    if (ret.y <= 0) {
                        ret.y = 1;
                    }
                    return ret;
                } else {
                    return projectionSize;
                }
            } else {
                return screenSize;
            }
        }

        Vec2f getMargins(const Vec2f &screenSize) const {
            return screenSize - getSize(screenSize.convert<float>());
        }

        void fitProjectionToScreen(const Vec2i &screenSize) {
            auto size = getSize(screenSize.convert<float>());
            auto margins = getMargins(screenSize.convert<float>());
            viewportOffset = margins.convert<int>() / 2;
            viewportSize = size.convert<int>();
        }
    };
}

#endif //XENGINE_CANVASCOMPONENT_HPP
