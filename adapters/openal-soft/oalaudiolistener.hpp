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

#ifndef XENGINE_OALAUDIOLISTENER_HPP
#define XENGINE_OALAUDIOLISTENER_HPP

#include "xng/audio/audiolistener.hpp"
#include "xng/audio/audiocontext.hpp"

namespace xng {
    namespace openal {
        class OALAudioListener : public AudioListener {
        public:
            OALAudioListener();

            ~OALAudioListener() override;

            void setGain(float gain) override;

            float getGain() override;

            void setPosition(Vec3f position) override;

            Vec3f getPosition() override;

            void setVelocity(Vec3f velocity) override;

            Vec3f getVelocity() override;

            void setOrientation(Vec3f at, Vec3f up) override;

            void getOrientation(Vec3f &at, Vec3f &up) override;
        };
    }
}

#endif //XENGINE_OALAUDIOLISTENER_HPP
