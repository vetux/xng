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
    SpriteAnimation::SpriteAnimation(std::vector<std::reference_wrapper<TextureBuffer>> frames,
                                     float animationDuration,
                                     bool loop,
                                     bool clampDelta)
            : frames(std::move(frames)),
              animationDuration(animationDuration),
              clampDelta(clampDelta),
              loop(loop) {
        frameTime = animationDuration / numeric_cast<float>(frames.size());
    }

    TextureBuffer &SpriteAnimation::getFrame(float deltaTime) {
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

        return frames.at(frame);
    }
}