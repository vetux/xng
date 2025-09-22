/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_ANDROIDINPUT_HPP
#define XENGINE_ANDROIDINPUT_HPP

#include "xng/input/input.hpp"

namespace xng {
    namespace android {
        class AndroidInput : public Input {
        public:
            void setClipboardText(std::string text) override {

            }

            std::string getClipboardText() override {
                return std::string();
            }

            void setMouseCursorImage(const ImageRGBA &image) override {

            }

            void clearMouseCursorImage() override {

            }

            void setMouseCursorHidden(bool cursorHidden) override {

            }

            void setEventBus(const EventBus &bus) override {

            }

            void clearEventBus() override {

            }

            const InputDevice &getDevice(std::type_index deviceType, int id) override {
                throw std::runtime_error("Unsupported device type " + std::string(deviceType.name()));
            }

            std::map<int, const std::reference_wrapper<InputDevice>> getDevices(std::type_index deviceType) override {
                throw std::runtime_error("Unsupported device type " + std::string(deviceType.name()));
            }
        };
    }
}

#endif //XENGINE_ANDROIDINPUT_HPP
