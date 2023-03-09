/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/asset/image.hpp"
#include "xng/util/listenable.hpp"

#include "xng/input/inputdevice.hpp"
#include "xng/input/inputlistener.hpp"

namespace xng {
    class XENGINE_EXPORT Input : public Listenable<InputListener> {
    public:
        virtual ~Input() = default;

        virtual void setClipboardText(std::string text) = 0;

        virtual std::string getClipboardText() = 0;

        virtual void setMouseCursorImage(const ImageRGBA &image) = 0;

        virtual void clearMouseCursorImage() = 0;

        virtual void setMouseCursorHidden(bool cursorHidden) = 0;

        virtual const InputDevice &getDevice(std::type_index deviceType, int id) = 0;

        template<typename T>
        const T &getDevice(int id = 0) {
            return dynamic_cast<const T&>(getDevice(typeid(T), id));
        }

        virtual std::map<int, const std::reference_wrapper<InputDevice>> getDevices(std::type_index deviceType) = 0;

        template<typename T>
        const std::map<int, std::reference_wrapper<T>> &getDevices(){
            std::map<int, std::reference_wrapper<T>> ret;
            for (auto pair : getDevices(typeid(T))){
                ret[pair.first] = dynamic_cast<T&>(pair.second.get());
            }
            return ret;
        }
    };
}

#endif //XENGINE_INPUT_HPP
