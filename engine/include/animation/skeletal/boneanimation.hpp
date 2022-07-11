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

#ifndef XENGINE_BONEANIMATION_HPP
#define XENGINE_BONEANIMATION_HPP

#include "math/quaternion.hpp"

namespace xng {
    /**
     * The set of keyframes of an animation for a bone.
     *
     * Keyframe data is relative to the origin so the resulting transformation replaces the default offset of the bone.
     */
    struct XENGINE_EXPORT BoneAnimation {
        enum Behaviour {
            DEFAULT, // The value from the default bone transformation is taken
            CONSTANT, //The nearest key value is used without interpolation.
            LINEAR, // The value of the nearest two keys is linearly extrapolated for the current time value.
            REPEAT // The animation is repeated. If the animation key go from n to m and the current time is t, use the value at (t-n) % (|m-n|).
        };

        // Define how the animation behaves outside the defined time range
        Behaviour preState;
        Behaviour postState;

        // The keyframes with their time in ticks as keys in a sorted map.
        std::map<double, Vec3f> positionFrames;
        std::map<double, Quaternion> rotationFrames;
        std::map<double, Vec3f> scaleFrames;
    };
}
#endif //XENGINE_BONEANIMATION_HPP
