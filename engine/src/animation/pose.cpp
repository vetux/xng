/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2026 Julia Zampiccoli
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

#include "xng/animation/pose.hpp"

#include "xng/math/interpolation.hpp"

namespace xng {
    Pose Pose::sample(const Duration &time,
                      const double ticksPerSecond,
                      const std::unordered_map<std::string, AnimationChannel> &channels) {
        Pose ret;
        for (auto &pair: channels) {
            const auto position = interpolate(time,
                                              ticksPerSecond,
                                              pair.second.preState,
                                              pair.second.postState,
                                              pair.second.positionFrames);
            const auto rotation = interpolate(time,
                                              ticksPerSecond,
                                              pair.second.preState,
                                              pair.second.postState,
                                              pair.second.rotationFrames);
            const auto scale = interpolate(time,
                                           ticksPerSecond,
                                           pair.second.preState,
                                           pair.second.postState,
                                           pair.second.scaleFrames);
            ret.transforms.emplace(pair.first, Transform(position, rotation, scale));
        }
        return ret;
    }

    Pose Pose::blend(const Pose &poseA, const Pose &poseB, const float weight) {
        Pose ret;
        for (auto &pair: poseA.transforms) {
            auto it = poseB.transforms.find(pair.first);
            if (it == poseB.transforms.end()) {
                ret.transforms.emplace(pair.first, pair.second);
                continue;
            }
            const auto &transformA = pair.second;
            const auto &transformB = it->second;
            ret.transforms.emplace(pair.first, blend(transformA, transformB, weight));
        }
        return ret;
    }

    Pose::Transform Pose::blend(const Transform &transformA,
                                    const Transform &transformB,
                                    const float weight) {
        Transform ret;
        ret.position = lerp(transformA.position, transformB.position, weight);
        ret.rotation = slerp(transformA.rotation, transformB.rotation, weight);
        ret.scale = lerp(transformA.scale, transformB.scale, weight);
        return ret;
    }

    Vec3f Pose::interpolate(const Duration &time,
                            const double ticksPerSecond,
                            const AnimationChannel::Behaviour preState,
                            const AnimationChannel::Behaviour postState,
                            const std::map<double, Vec3f> &frames) {
        assert(!frames.empty());

        const auto tickDur = (1 / ticksPerSecond);
        const auto ticks = time / tickDur;

        if (ticks < frames.begin()->first) {
            switch (preState) {
                default:
                case AnimationChannel::LINEAR: {
                    if (frames.size() < 2) return frames.begin()->second;
                    const auto itA = frames.begin();
                    const auto itB = std::next(itA);
                    const auto totalDiff = itB->first - itA->first;
                    const auto ticksDiff = ticks - itA->first; // negative
                    return lerp(itA->second, itB->second, static_cast<float>(ticksDiff / totalDiff));
                }
            }
        }

        if (ticks > frames.rbegin()->first) {
            switch (postState) {
                default:
                case AnimationChannel::LINEAR: {
                    if (frames.size() < 2) return frames.rbegin()->second;
                    auto itB = frames.end();
                    --itB;
                    const auto itA = std::prev(itB);
                    const auto totalDiff = itB->first - itA->first;
                    const auto ticksDiff = ticks - itA->first; // > totalDiff
                    return lerp(itA->second, itB->second, static_cast<float>(ticksDiff / totalDiff));
                }
            }
        }

        const auto itB = frames.lower_bound(ticks);
        const auto itA = std::prev(itB);
        const auto totalDiff = itB->first - itA->first;
        const auto ticksDiff = ticks - itA->first;
        return lerp(itA->second, itB->second, static_cast<float>(ticksDiff / totalDiff));
    }

    Quaternion Pose::interpolate(const Duration &time,
                                 const double ticksPerSecond,
                                 const AnimationChannel::Behaviour preState,
                                 const AnimationChannel::Behaviour postState,
                                 const std::map<double, Quaternion> &frames) {
        assert(!frames.empty());

        const auto tickDur = (1 / ticksPerSecond);
        const auto ticks = time / tickDur;

        if (ticks < frames.begin()->first) {
            switch (preState) {
                default:
                case AnimationChannel::LINEAR: {
                    if (frames.size() < 2) return frames.begin()->second;
                    const auto itA = frames.begin();
                    const auto itB = std::next(itA);
                    const auto totalDiff = itB->first - itA->first;
                    const auto ticksDiff = ticks - itA->first;
                    return slerp(itA->second, itB->second, static_cast<float>(ticksDiff / totalDiff)).normalize();
                }
            }
        }

        if (ticks > frames.rbegin()->first) {
            switch (postState) {
                default:
                case AnimationChannel::LINEAR: {
                    if (frames.size() < 2) return frames.rbegin()->second;
                    auto itB = frames.end();
                    --itB;
                    const auto itA = std::prev(itB);
                    const auto totalDiff = itB->first - itA->first;
                    const auto ticksDiff = ticks - itA->first;
                    return slerp(itA->second, itB->second, static_cast<float>(ticksDiff / totalDiff)).normalize();
                }
            }
        }

        const auto itB = frames.lower_bound(ticks);
        const auto itA = std::prev(itB);
        const auto totalDiff = itB->first - itA->first;
        const auto ticksDiff = ticks - itA->first;
        return slerp(itA->second, itB->second, static_cast<float>(ticksDiff / totalDiff)).normalize();
    }
}
