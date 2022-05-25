/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "audio/openal/oalaudiodriver.hpp"

#include <stdexcept>

#include "audio/openal/oalaudiocontext.hpp"
#include "audio/openal/oalcheckerror.hpp"

#include "audio/openal/oalaudiodevice.hpp"

#include "driver/drivermanager.hpp"

namespace xengine {
    static bool dr = REGISTER_DRIVER("openal-soft", OALAudioDriver);

    std::vector<std::string> OALAudioDriver::getDeviceNames() {
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

    std::unique_ptr<AudioDevice> OALAudioDriver::createDevice() {
        return std::make_unique<OALAudioDevice>();
    }

    std::unique_ptr<AudioDevice> OALAudioDriver::createDevice(const std::string &deviceName) {
        return std::make_unique<OALAudioDevice>(deviceName);
    }

    std::type_index OALAudioDriver::getType() {
        return typeid(OALAudioDriver);
    }
}