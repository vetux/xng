/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_POSE_HPP
#define XENGINE_POSE_HPP

#include <string>
#include <unordered_map>

#include "xng/math/quaternion.hpp"
#include "xng/util/time.hpp"

#include "xng/animation/skeletal/boneanimation.hpp"

namespace xng {
    struct XENGINE_EXPORT Pose {
        struct BoneTransform {
            Vec3f position;
            Quaternion rotation;
            Vec3f scale;

            BoneTransform() = default;

            BoneTransform(Vec3f position, Quaternion rotation, Vec3f scale)
                : position(std::move(position)), rotation(std::move(rotation)), scale(std::move(scale)) {
            }
        };

        std::unordered_map<std::string, BoneTransform> transforms;

        /**
         * @param time The time point to sample
         * @param ticksPerSecond The number of ticks to show per second
         * @param channels The animation channels keyed by bone name
         * @return The animated pose for the given time point
         */
        static Pose sample(const Duration &time,
                           double ticksPerSecond,
                           const std::unordered_map<std::string, BoneAnimation> &channels);

        /**
         * Blend poseA and poseB with the given weight.
         *
         * weight == 0 = poseA
         * weight == 1 = poseB
         *
         * @param poseA
         * @param poseB
         * @param weight
         * @return
         */
        static Pose blend(const Pose &poseA, const Pose &poseB, float weight);

        static BoneTransform blend(const BoneTransform &transformA,
                                   const BoneTransform &transformB,
                                   float weight);

        static Vec3f interpolate(const Duration &time,
                                 double ticksPerSecond,
                                 BoneAnimation::Behaviour preState,
                                 BoneAnimation::Behaviour postState,
                                 const std::map<double, Vec3f> &frames);

        static Quaternion interpolate(const Duration &time,
                                      double ticksPerSecond,
                                      BoneAnimation::Behaviour preState,
                                      BoneAnimation::Behaviour postState,
                                      const std::map<double, Quaternion> &frames);
    };
}

#endif //XENGINE_POSE_HPP
