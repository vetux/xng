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

#ifndef XENGINE_AUDIODRIVER_HPP
#define XENGINE_AUDIODRIVER_HPP

#include "xng/driver/driver.hpp"

#include "audiodevice.hpp"

namespace xng {
    enum AudioDriverBackend {
        OPENAL_SOFT
    };

    class XENGINE_EXPORT AudioDriver : public Driver {
    public:
        static std::unique_ptr<AudioDriver> load(AudioDriverBackend backend) {
            switch (backend) {
                case OPENAL_SOFT:
                    return std::unique_ptr<AudioDriver>(
                            dynamic_cast<AudioDriver *>(Driver::load("openal-soft").release()));
            }
            throw std::runtime_error("Invalid backend");
        }

        virtual std::vector<std::string> getDeviceNames() = 0;

        virtual std::unique_ptr<AudioDevice> createDevice() = 0;

        virtual std::unique_ptr<AudioDevice> createDevice(const std::string &deviceName) = 0;

    private:
        std::type_index getBaseType() override {
            return typeid(AudioDriver);
        }
    };
}
#endif //XENGINE_AUDIODRIVER_HPP
