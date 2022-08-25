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

#ifndef XENGINE_INPUT_HPP
#define XENGINE_INPUT_HPP

#include <set>

#include "input/inputlistener.hpp"
#include "asset/image.hpp"
#include "util/listenable.hpp"

namespace xng {
    class XENGINE_EXPORT Input : public Listenable<InputListener> {
    public:
        virtual ~Input() = default;

        virtual void setClipboardText(std::string text) = 0;

        virtual std::string getClipboardText() = 0;

        virtual void setMouseCursorImage(const ImageRGBA &image) = 0;

        virtual void clearMouseCursorImage() = 0;

        virtual void setMouseCursorHidden(bool cursorHidden) = 0;

        virtual const Keyboard &getKeyboard() const = 0;

        virtual const Mouse &getMouse() const = 0;

        virtual const GamePad &getGamePad() const = 0;

        virtual const std::map<int, Keyboard> &getKeyboards() const = 0;

        virtual const std::map<int, Mouse> &getMice() const = 0;

        virtual const std::map<int, GamePad> &getGamePads() const = 0;
    };
}

#endif //XENGINE_INPUT_HPP
