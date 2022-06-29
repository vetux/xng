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

#ifndef XENGINE_AUDIOSOURCECOMPONENT_HPP
#define XENGINE_AUDIOSOURCECOMPONENT_HPP

#include "asset/audio.hpp"
#include "math/vector3.hpp"
#include "resource/resourcehandle.hpp"

namespace xng {
    struct XENGINE_EXPORT AudioSourceComponent {
        ResourceHandle<Audio> audio;
        bool play = false;
        bool loop = false;
        Vec3f velocity = {};

        bool playing = false;

        bool operator==(const AudioSourceComponent &other) const {
            return audio == other.audio
                   && play == other.play
                   && loop == other.loop
                   && velocity == other.velocity
                   && playing == other.playing;
        }

        bool operator!=(const AudioSourceComponent &other) const {
            return !(*this == other);
        }
    };
}

#endif //XENGINE_AUDIOSOURCECOMPONENT_HPP
