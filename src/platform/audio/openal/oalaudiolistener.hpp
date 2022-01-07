/**
 *  Mana - 3D Game Engine
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

#ifndef MANA_OALAUDIOLISTENER_HPP
#define MANA_OALAUDIOLISTENER_HPP

#include "platform/audio/audiolistener.hpp"
#include "platform/audio/audiocontext.hpp"

namespace xengine {
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

#endif //MANA_OALAUDIOLISTENER_HPP
