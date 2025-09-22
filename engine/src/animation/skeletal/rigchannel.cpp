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

#include "xng/animation/skeletal/rigchannel.hpp"

namespace xng {
    std::vector<RigKeyframe> RigChannel::update(DeltaTime deltaTime) {
        auto incDeltaTime = deltaTime * animationSpeed;

        time += incDeltaTime;

        if (animations.empty())
            return {};

        std::vector<RigKeyframe> ret;

        auto currentAnimation = animations.at(0);

        RigKeyframe kf;
        kf.boneMask = boneMask;
        kf.animation = currentAnimation.animation;

        currentAnimation.fadeTime += incDeltaTime;

        if (currentAnimation.fadeTime >= currentAnimation.blendDur) {
            currentAnimation.fadeTime = currentAnimation.blendDur;
            kf.weight = 1;
        } else {
            kf.weight = currentAnimation.fadeTime / currentAnimation.blendDur;
        }

        kf.loop = currentAnimation.loop;
        kf.time = time;

        ret.emplace_back(kf);

        auto longestAnimation = static_cast<Duration>(currentAnimation.animation.duration
                                                      / currentAnimation.animation.ticksPerSecond);

        auto blendRate = (1 / currentAnimation.blendDur);

        if (animations.size() > 1) {
            for (auto fadeAnimation = animations.begin() + 1; fadeAnimation != animations.end(); fadeAnimation++) {
                fadeAnimation->fadeTime -= fadeAnimation->blendDur * blendRate * deltaTime * animationSpeed;

                if (fadeAnimation->fadeTime <= 0) {
                    fadeAnimation->fadeTime = 0;
                    continue;
                } else {
                    kf.weight = fadeAnimation->fadeTime / fadeAnimation->blendDur;
                }

                kf.animation = fadeAnimation->animation;

                kf.loop = fadeAnimation->loop;
                kf.time = time;

                ret.emplace_back(kf);
            }

            while (animations.size() > 1) {
                if ((animations.end() - 1)->fadeTime == 0) {
                    animations.pop_back();
                } else {
                    break;
                }
            }

            for (auto fadeAnimation = animations.begin() + 1; fadeAnimation != animations.end(); fadeAnimation++) {
                auto dur = static_cast<Duration>(fadeAnimation->animation.duration
                                                 / fadeAnimation->animation.ticksPerSecond);

                if (dur > longestAnimation) {
                    longestAnimation = dur;
                }
            }
        }

        if (time >= longestAnimation) {
            time = 0;
        }

        return ret;
    }

    void RigChannel::start(const RigAnimation &animation, Duration blendDur, bool loop) {
        Animation anim;
        anim.animation = animation;
        anim.loop = loop;
        anim.blendDur = blendDur;
        anim.fadeTime = blendDur;
        animations.insert(animations.begin(), anim);
    }
}