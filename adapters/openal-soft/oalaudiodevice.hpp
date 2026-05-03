/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_OALAUDIODEVICE_HPP
#define XENGINE_OALAUDIODEVICE_HPP

#include "openalinclude.hpp"

#include "xng/audio/audiodevice.hpp"

namespace xng {
    namespace openal {
        class OALAudioDevice : public AudioDevice {
        public:
            static std::vector<std::string> getDeviceNames();

            OALAudioDevice();

            explicit OALAudioDevice(const std::string &name);

            ~OALAudioDevice() override;

            std::unique_ptr<AudioContext> createContext() override;

        private:
            ALCdevice *device;
        };
    }
}

#endif //XENGINE_OALAUDIODEVICE_HPP
