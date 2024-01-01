/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_RIGCHANNEL_HPP
#define XENGINE_RIGCHANNEL_HPP

#include "xng/animation/skeletal/rigkeyframe.hpp"

#include "xng/util/time.hpp"

namespace xng {
    class RigChannel {
    public:
        std::vector<RigKeyframe> update(DeltaTime deltaTime);

        void start(const RigAnimation &animation, Duration blendDur = {}, bool loop = true);

        void setAnimationSpeed(float speed) {
            animationSpeed = speed;
        }

        Duration getTime() {
            return time;
        }

        /**
         * Set the list of bones which this channel can affect.
         * If empty the channel will affect all bones.
         *
         * Alternatively the bones can be excluded when creating the animations.
         *
         * @param bones
         */
        void setBoneMask(const std::set<std::string> &bones) {
            boneMask = bones;
        }

    private:
        struct Animation {
            RigAnimation animation;
            bool loop{};
            Duration blendDur{};
            Duration time{};
            Duration fadeTime{};
        };

        std::vector<Animation> animations;

        float animationSpeed = 1;

        Duration time{}; // The accumulated time in seconds, resets when larger than the largest duration of the animations.

        std::set<std::string> boneMask;
    };
}

#endif //XENGINE_RIGCHANNEL_HPP
