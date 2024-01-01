/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include <utility>
#include <cmath>

#include "xng/animation/skeletal/riganimator.hpp"
#include "xng/util/time.hpp"

#include "xng/math/matrixmath.hpp"
#include "xng/math/interpolation.hpp"

namespace xng {
    // TODO: Redesign channel mixing
    // TODO: Implement bone animation behaviours
    // TODO: Implement animation loop control

    static Vec3f interpolatePosition(const Duration &time,
                                     const BoneAnimation &animation,
                                     double ticksPerSecond) {
        assert(!animation.positionFrames.empty());

        auto tickDur = (1 / ticksPerSecond);
        auto ticks = time / tickDur;

        if (ticks < animation.positionFrames.begin()->first) {
            switch (animation.preState) {
                default:
                case BoneAnimation::LINEAR: {
                    auto posA = (animation.positionFrames.rbegin())->first;
                    auto posB = animation.positionFrames.begin()->first;

                    auto frameA = animation.positionFrames.at(posA);
                    auto frameB = animation.positionFrames.at(posB);

                    auto totalDiff = posB - posA;
                    auto ticksDiff = ticks - posA;

                    return lerp(frameA, frameB, static_cast<float>(ticksDiff / totalDiff));
                }
            }
        } else if (ticks > animation.positionFrames.rbegin()->first) {
            switch (animation.postState) {
                default:
                case BoneAnimation::LINEAR: {
                    auto posA = (animation.positionFrames.rbegin())->first;
                    auto posB = animation.positionFrames.begin()->first;

                    auto frameA = animation.positionFrames.at(posA);
                    auto frameB = animation.positionFrames.at(posB);

                    auto totalDiff = posA - posB;
                    auto ticksDiff = ticks - posA;

                    return lerp(frameA, frameB, static_cast<float>(ticksDiff / totalDiff));
                }
            }
        } else {
            auto posA = animation.getNearestPositionKey(ticks);

            auto itB = ++animation.positionFrames.find(posA);
            assert(itB != animation.positionFrames.end());

            auto posB = itB->first;

            auto frameA = animation.positionFrames.at(posA);
            auto frameB = animation.positionFrames.at(posB);

            auto totalDiff = posB - posA;
            auto ticksDiff = ticks - posA;

            return lerp(frameA, frameB, static_cast<float>(ticksDiff / totalDiff));
        }
    }

    static Quaternion interpolateRotation(const Duration &time,
                                          const BoneAnimation &animation,
                                          double ticksPerSecond) {
        assert(!animation.rotationFrames.empty());

        auto tickDur = (1 / ticksPerSecond);
        auto ticks = time / tickDur;

        if (ticks < animation.rotationFrames.begin()->first) {
            switch (animation.preState) {
                default:
                case BoneAnimation::LINEAR: {
                    auto posA = (animation.rotationFrames.rbegin())->first;
                    auto posB = animation.positionFrames.begin()->first;

                    auto frameA = animation.rotationFrames.at(posA);
                    auto frameB = animation.rotationFrames.at(posB);

                    auto totalDiff = posB - posA;
                    auto ticksDiff = ticks - posA;

                    return slerp(frameA, frameB, static_cast<float>(ticksDiff / totalDiff)).normalize();
                }
            }
        } else if (ticks > animation.rotationFrames.rbegin()->first) {
            switch (animation.postState) {
                default:
                case BoneAnimation::LINEAR: {
                    auto posA = (animation.rotationFrames.rbegin())->first;
                    auto posB = animation.rotationFrames.begin()->first;

                    auto frameA = animation.rotationFrames.at(posA);
                    auto frameB = animation.rotationFrames.at(posB);

                    auto totalDiff = posA - posB;
                    auto ticksDiff = ticks - posA;

                    return slerp(frameA, frameB, static_cast<float>(ticksDiff / totalDiff)).normalize();
                }
            }
        } else {
            auto posA = animation.getNearestRotationKey(ticks);

            auto itB = ++animation.rotationFrames.find(posA);
            assert(itB != animation.rotationFrames.end());

            auto posB = itB->first;

            auto frameA = animation.rotationFrames.at(posA);
            auto frameB = animation.rotationFrames.at(posB);

            auto totalDiff = posB - posA;
            auto ticksDiff = ticks - posA;

            return slerp(frameA, frameB, static_cast<float>(ticksDiff / totalDiff)).normalize();
        }
    }

    static Vec3f interpolateScale(const Duration &time,
                                  const BoneAnimation &animation,
                                  double ticksPerSecond) {
        assert(!animation.scaleFrames.empty());

        auto tickDur = (1 / ticksPerSecond);
        auto ticks = time / tickDur;

        if (ticks < animation.scaleFrames.begin()->first) {
            switch (animation.preState) {
                default:
                case BoneAnimation::LINEAR: {
                    auto posA = (animation.scaleFrames.rbegin())->first;
                    auto posB = animation.scaleFrames.begin()->first;

                    auto frameA = animation.scaleFrames.at(posA);
                    auto frameB = animation.scaleFrames.at(posB);

                    auto totalDiff = posB - posA;
                    auto ticksDiff = ticks - posA;

                    return lerp(frameA, frameB, static_cast<float>(ticksDiff / totalDiff));
                }
            }
        } else if (ticks > animation.scaleFrames.rbegin()->first) {
            switch (animation.postState) {
                default:
                case BoneAnimation::LINEAR: {
                    auto posA = (animation.scaleFrames.rbegin())->first;
                    auto posB = animation.scaleFrames.begin()->first;

                    auto frameA = animation.scaleFrames.at(posA);
                    auto frameB = animation.scaleFrames.at(posB);

                    auto totalDiff = posA - posB;
                    auto ticksDiff = ticks - posA;

                    return lerp(frameA, frameB, static_cast<float>(ticksDiff / totalDiff));
                }
            }
        } else {
            auto posA = animation.getNearestScaleKey(ticks);

            auto itB = ++animation.scaleFrames.find(posA);
            assert(itB != animation.scaleFrames.end());

            auto posB = itB->first;

            auto frameA = animation.scaleFrames.at(posA);
            auto frameB = animation.scaleFrames.at(posB);

            auto totalDiff = posB - posA;
            auto ticksDiff = ticks - posA;

            return lerp(frameA, frameB, static_cast<float>(ticksDiff / totalDiff));
        }
    }

    RigAnimator::RigAnimator(Rig rig)
            : rig(std::move(rig)) {
    }

    static void getTransformRecursive(Bone &bone,
                                      Rig &rig,
                                      Mat4f parentTransform,
                                      std::map<std::string, Mat4f> &boneTransforms,
                                      const std::map<std::string, Mat4f> &keyframes) {
        auto it = keyframes.find(bone.name);

        Mat4f localTransform;
        if (it != keyframes.end()) {
            localTransform = it->second;
        } else {
            localTransform = bone.transform;
        }

        Mat4f globalTransform = parentTransform * localTransform;

        boneTransforms[bone.name] = globalTransform * bone.offset;

        for (auto &childBone: rig.getChildBones(bone.name)) {
            getTransformRecursive(childBone.get(), rig, globalTransform, boneTransforms, keyframes);
        }
    }

    struct Keyframe {
        Vec3f position{};
        Quaternion rotation{};
        Vec3f scale{};
        float weight{};
    };

    void RigAnimator::update(DeltaTime deltaTime) {
        boneTransforms.clear();

        std::vector<RigKeyframe> channelFrames;
        for (auto &pair: channels) {
            auto vec = pair.second.update(deltaTime);
            channelFrames.insert(channelFrames.end(), vec.begin(), vec.end());
        }

        if (channelFrames.empty())
            return;


        std::map<std::string, std::vector<Keyframe>> keyframes;

        float totalWeight = 0;
        for (auto &channelFrame: channelFrames) {
            totalWeight += channelFrame.weight;
            for (auto &boneAnimation: channelFrame.animation.channels) {
                auto pos = interpolatePosition(channelFrame.time,
                                               boneAnimation,
                                               channelFrame.animation.ticksPerSecond);
                auto rot = interpolateRotation(channelFrame.time,
                                               boneAnimation,
                                               channelFrame.animation.ticksPerSecond).normalize();
                auto scale = interpolateScale(channelFrame.time,
                                              boneAnimation,
                                              channelFrame.animation.ticksPerSecond);

                Keyframe kf;
                kf.position = pos;
                kf.rotation = rot;
                kf.scale = scale;
                kf.weight = channelFrame.weight;
                keyframes[boneAnimation.name].emplace_back(kf);
            }
        }

        std::map<std::string, Mat4f> localTransforms;
        for (auto &pair: keyframes) {
            assert(!pair.second.empty());

            if (pair.second.size() > 1) {
                auto kfA = pair.second.begin();
                auto kfB = pair.second.begin()++;

                auto wA = kfA->weight / totalWeight;
                auto wB = kfB->weight / totalWeight;

                auto t = wA + wB;

                auto totalPos = lerp(kfA->position, kfB->position, t);
                auto totalRot = slerp(kfA->rotation, kfB->rotation, t);
                auto totalScale = lerp(kfA->scale, kfB->scale, t);

                for (auto kfC = kfB++; kfC != pair.second.end(); kfC++) {
                    auto wC = kfC->weight / totalWeight;
                    totalPos = lerp(totalPos, kfC->position, wC);
                    totalRot = slerp(totalRot, kfC->rotation, wC);
                    totalScale = lerp(totalScale, kfC->scale, wC);
                }

                auto transform = MatrixMath::translate(totalPos)
                                 * totalRot.matrix()
                                 * MatrixMath::scale(totalScale);

                if (localTransforms.find(pair.first) == localTransforms.end()) {
                    localTransforms[pair.first] = MatrixMath::identity();
                }

                localTransforms[pair.first] = transform * localTransforms[pair.first];
            } else {
                auto &kf = *pair.second.begin();

                auto transform = MatrixMath::translate(kf.position)
                                 * kf.rotation.matrix()
                                 * MatrixMath::scale(kf.scale);

                if (localTransforms.find(pair.first) == localTransforms.end()) {
                    localTransforms[pair.first] = MatrixMath::identity();
                }

                localTransforms[pair.first] = transform * localTransforms[pair.first];
            }
        }

        for (auto &bone: rig.getRootBones()) {
            getTransformRecursive(bone, rig, MatrixMath::identity(), boneTransforms, localTransforms);
        }
    }

    void RigAnimator::start(const RigAnimation &animation, Duration blendDur, bool loop, size_t channel) {
        channels[channel].start(animation, blendDur, loop);
    }

    void RigAnimator::stop(size_t channel) {
        channels.erase(channel);
    }

    void RigAnimator::setAnimationSpeed(float speed, size_t channel) {
        channels[channel].setAnimationSpeed(speed);
    }
}
