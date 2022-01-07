/**
 *  XEngine - C++ game engine library
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

#ifdef BUILD_ENGINE_AUDIO_OPENAL

#include <stdexcept>

#include "oalaudiocontext.hpp"
#include "oalcheckerror.hpp"

#include "oalaudiodevice.hpp"

namespace xengine {
    std::vector<std::string> OALAudioDevice::getDeviceNames() {
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

    OALAudioDevice::OALAudioDevice() {
        device = alcOpenDevice(nullptr);
        if (!device) {
            throw std::runtime_error("Failed to open default device");
        }
    }

    OALAudioDevice::OALAudioDevice(const std::string &name) {
        device = alcOpenDevice(name.c_str());
        if (!device) {
            throw std::runtime_error("Failed to open device " + name);
        }
    }

    OALAudioDevice::~OALAudioDevice() {
        alcCloseDevice(device);
    }

    std::unique_ptr<AudioContext> OALAudioDevice::createContext() {
        return std::make_unique<OALAudioContext>(alcCreateContext(device, nullptr));
    }
}

#endif