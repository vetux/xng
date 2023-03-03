/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "xng/types/deltatime.hpp"

namespace xng {
    /**
     * A rig animator applies animations to a rig.
     */
    class XENGINE_EXPORT RigAnimator {
    public:
        enum BlendOp {
            SWITCH, // Show the first keyframe of the next animation without interpolation
            CROSS_FADE, // Fade the last frame of the current animation with the frames of the next animation
        };

        RigAnimator() = default;

        RigAnimator(Rig rig, bool loop);

        /**
         * Advance the animation by deltaTime
         *
         * @param deltaTime
         */
        void update(DeltaTime deltaTime);

        /**
         * Start the specified animation and if an animation is running use the blendOp for blending between
         * the currently running animation and the passed animation.
         *
         * @param index
         * @param blendOp
         */
        void start(std::shared_ptr<RigAnimation> animation, BlendOp blendOp);

        /**
         * Set the animated rig to the default rig and remove the reference to the animation object.
         */
        void stop();

        /**
         * Reset the time value to 0 and beginFrame the animated rig.
         */
        void reset();

        /**
         * @return The rig object with the bone offsets containing the animated values.
         */
        const Rig &getRig() const;

        void setLooping(bool looping);

        bool isLooping() const;

        float getDuration() const;

        float getTime() const;

    private:
        Rig defRig;
        Rig animRig;
        bool loop = true;

        std::shared_ptr<RigAnimation> animation;
        float duration; // The duration of the animation in seconds.
        float time; // The accumulated time in seconds, never larger than duration.
        BlendOp blend;
    };
}

#endif //XENGINE_RIGANIMATOR_HPP
