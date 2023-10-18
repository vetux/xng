/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_RIGANIMATOR_HPP
#define XENGINE_RIGANIMATOR_HPP

#include <memory>

#include "rig.hpp"
#include "riganimation.hpp"

#include "xng/types/time.hpp"

#include "xng/animation/skeletal/rigchannel.hpp"

namespace xng {
    /**
     * A rig animator applies animations to a rig using weighted animations.
     *
     * Each channel handles the blending between running animations
     * and represents an animation layer for combining multiple animations for example "walk" and "shoot".
     *
     * Channel:
     *
     * (keyframeA.animation.value * keyframeA.weight) + (keyframeB.animation.value * keyframeB.weight) + ...
     *
     * Final Result:
     *
     * channelA + channelB + ...
     *
     */
    class XENGINE_EXPORT RigAnimator {
    public:
        RigAnimator() = default;

        RigAnimator(Rig rig);

        /**
         * Advance the animation channels by deltaTime and update the animated rig.
         *
         * @param deltaTime
         */
        void update(DeltaTime deltaTime);

        /**
         *
         * @param animation
         * @param blendDur
         * @param loop
         * @param channel
         */
        void start(const RigAnimation &animation, Duration blendDur = {}, bool loop = true, size_t channel = 0);

        /**
         *
         * @param channel
         */
        void stop(size_t channel = 0);

        /**
         *
         * @param speed
         * @param channel
         */
        void setAnimationSpeed(float speed, size_t channel = 0);

        const std::map<std::string, Mat4f> &getBoneTransforms() { return boneTransforms; }

    private:
        Rig rig;

        std::map<std::string, Mat4f> boneTransforms;

        std::map<size_t, RigChannel> channels;
    };
}

#endif //XENGINE_RIGANIMATOR_HPP
