/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include "xng/platform/openal-soft/openal.hpp"

#include <stdexcept>

#include "audio/openal/oalaudiocontext.hpp"
#include "audio/openal/oalcheckerror.hpp"
#include "audio/openal/oalaudiodevice.hpp"

namespace xng {
    namespace openal {
        std::vector<std::string> OpenAL::getDeviceNames() {
            const char *dev = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
            std::vector<std::string> ret;
            std::string tmp;
            bool gotZero = false;
            for (int i = 0; i < ((std::string) dev).size(); i++) {
                char c = dev[i];
                if (c == 0) {
                    if (gotZero)
                        break;
                    gotZero = true;
                    ret.emplace_back(tmp);
                    tmp.clear();
                } else {
                    gotZero = false;
                    tmp += c;
                }
            }
            return ret;
        }

        std::unique_ptr<AudioDevice> OpenAL::createDevice() {
            return std::make_unique<OALAudioDevice>();
        }

        std::unique_ptr<AudioDevice> OpenAL::createDevice(const std::string &deviceName) {
            return std::make_unique<OALAudioDevice>(deviceName);
        }
    }
}