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

#include "platform/audio/audiodevice.hpp"

#include <stdexcept>

#ifdef BUILD_ENGINE_AUDIO_OPENAL
#include "openal/oalaudiodevice.hpp"
#endif

namespace xengine {
    std::vector<std::string> AudioDevice::getDeviceNames(AudioBackend backend) {
        switch (backend) {
#ifdef BUILD_ENGINE_AUDIO_OPENAL
            case OpenAL:
                return OALAudioDevice::getDeviceNames();
#endif
            default:
                throw std::runtime_error("Unsupported audio backend");
        }
    }

    std::unique_ptr<AudioDevice> AudioDevice::createDevice(AudioBackend backend, const std::string &name) {
        switch (backend) {
#ifdef BUILD_ENGINE_AUDIO_OPENAL
            case OpenAL:
                return std::make_unique<OALAudioDevice>(name);
#endif
            default:
                throw std::runtime_error("Unsupported audio backend");
        }
    }

    std::unique_ptr<AudioDevice> AudioDevice::createDevice(AudioBackend backend) {
        switch (backend) {
#ifdef BUILD_ENGINE_AUDIO_OPENAL
            case OpenAL:
                return std::make_unique<OALAudioDevice>();
#endif
            default:
                throw std::runtime_error("Unsupported audio backend");
        }
    }
}
