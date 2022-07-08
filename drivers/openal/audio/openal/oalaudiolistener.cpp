/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "oalaudiolistener.hpp"
#include "oalcheckerror.hpp"

#include "openal.hpp"

namespace xng {
    OALAudioListener::OALAudioListener() {}

    OALAudioListener::~OALAudioListener() = default;

    void OALAudioListener::setGain(float gain) {
        alListenerf(AL_GAIN, gain);
        checkOALError();
    }

    float OALAudioListener::getGain() {
        float ret;
        alGetListenerf(AL_GAIN, &ret);
        checkOALError();
        return ret;
    }

    void OALAudioListener::setPosition(Vec3f position) {
        alListener3f(AL_POSITION, position.x, position.y, position.z);
        checkOALError();
    }

    Vec3f OALAudioListener::getPosition() {
        Vec3f ret;
        alGetListener3f(AL_POSITION, &ret.x, &ret.y, &ret.z);
        checkOALError();
        return ret;
    }

    void OALAudioListener::setVelocity(Vec3f velocity) {
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
        checkOALError();
    }

    Vec3f OALAudioListener::getVelocity() {
        Vec3f ret;
        alGetListener3f(AL_VELOCITY, &ret.x, &ret.y, &ret.z);
        checkOALError();
        return ret;
    }

    void OALAudioListener::setOrientation(Vec3f at, Vec3f up) {
        float v[6] = {at.x, at.y, at.z, up.x, up.y, up.z};
        alListenerfv(AL_ORIENTATION, v);
        checkOALError();
    }

    void OALAudioListener::getOrientation(Vec3f &at, Vec3f &up) {
        float v[6] = {at.x, at.y, at.z, up.x, up.y, up.z};
        alGetListenerfv(AL_ORIENTATION, v);
        checkOALError();
        at.x = v[0];
        at.y = v[1];
        at.z = v[2];
        up.x = v[3];
        up.y = v[4];
        up.z = v[5];
    }
}