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

#include "animation/sprite/spriteanimation.hpp"

#include <cmath>
#include <cassert>

namespace xng {
    SpriteAnimation::SpriteAnimation(std::vector<SpriteKeyframe> inputFrames,
                                     float animationDuration,
                                     bool loop,
                                     bool clampDelta)
            : keyframes(std::move(inputFrames)),
              animationDuration(animationDuration),
              clampDelta(clampDelta),
              loop(loop) {
        frameTime = animationDuration / numeric_cast<float>(keyframes.size());
        for (auto i = 0; i < keyframes.size(); i++) {
            auto &keyframe = keyframes.at(i);
            for (int v = 0; v < keyframe.duration; v++) {
                frames.emplace_back(keyframes.begin() + i);
            }
        }
    }

    SpriteAnimation::SpriteAnimation(std::vector<SpriteKeyframe> inputFrames,
                                     int animationFps,
                                     bool loop,
                                     bool clampDelta)
            : keyframes(std::move(inputFrames)),
              clampDelta(clampDelta),
              loop(loop) {
        int totalFrames = 0;
        for (auto &v: keyframes)
            totalFrames += v.duration;
        animationDuration = (1.0f / numeric_cast<float>(animationFps)) * numeric_cast<float>(totalFrames);
        frameTime = animationDuration / numeric_cast<float>(keyframes.size());
        for (auto i = 0; i < keyframes.size(); i++) {
            auto &keyframe = keyframes.at(i);
            for (int v = 0; v < keyframe.duration; v++) {
                frames.emplace_back(keyframes.begin() + i);
            }
        }
    }

    const ResourceHandle<Sprite> &SpriteAnimation::getFrame(float deltaTime) {
        if (frames.empty())
            throw std::runtime_error("Animation not initialized");

        if (!loop || currentTime < animationDuration) {
            if (clampDelta) {
                if (deltaTime > frameTime) {
                    // If deltaTime is larger than frameTime we would skip frames therefore we clamp deltaTime to frameTime
                    // which makes the animation advance at most one frame per call to getFrame.
                    deltaTime = frameTime;
                }
            }

            currentTime += deltaTime;

            if (currentTime > animationDuration) {
                if (loop) {
                    // Set currentTime to difference
                    currentTime = currentTime - animationDuration;
                } else {
                    currentTime = animationDuration;
                }
            }
        }

        auto frame = numeric_cast<size_t>(currentTime / frameTime);
        assert(frame < frames.size());
        return frames.at(frame)->sprite;
    }
}