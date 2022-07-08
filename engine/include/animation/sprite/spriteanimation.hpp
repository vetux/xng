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

#ifndef XENGINE_SPRITEANIMATION_HPP
#define XENGINE_SPRITEANIMATION_HPP

#include "animation/sprite/spritekeyframe.hpp"

namespace xng {
    /**
     * A sprite animation is a sequence of frames which are displayed sequentially to create the illusion of an animation.
     */
    class XENGINE_EXPORT SpriteAnimation {
    public:
        SpriteAnimation() = default;

        /**
         * @param frames The keyframes to use in order.
         * @param animationDuration The duration in seconds in which all the frames (Including duration frames of each key frame) should be displayed,
         * where every frame gets the same amount of display time. If clampDelta is true the animation may take more time but never less.
         * @param loop If true the animation will repeat.
         * @param clampDelta If true the animation will clamp deltaTime to frame time,
         * causing the animation to be advanced at most one frame per call to getFrame.
         * This causes animations to slow down when the framerate drops below the animation framerate.
         * The user can verify that an animation has finished by calling getCurrentTime()
         */
        SpriteAnimation(std::vector<SpriteKeyframe> frames,
                        float animationDuration,
                        bool loop = true,
                        bool clampDelta = false);

        SpriteAnimation(std::vector<SpriteKeyframe> frames,
                        int animationFps,
                        bool loop = true,
                        bool clampDelta = false);

        /**
         * Retrieve the current frame of the animation.
         *
         * @param deltaTime The passed time in seconds. If 0 no change of frame is made.
         * @return The texture corresponding to the current animation frame.
         */
        const ResourceHandle<Sprite> &getFrame(float deltaTime = 0);

        const std::vector<SpriteKeyframe> &getKeyframes() { return keyframes; }

        /**
         * @return The duration of the animation
         */
        float getDuration() const { return animationDuration; }

        bool isClampingDelta() const { return clampDelta; }

        bool isLooping() const { return loop; }

        /**
         * @return duration / frames.size()
         */
        float getFrameTime() const { return frameTime; }

        /**
         * @return The current total animation time. If currentTime == animationDuration and !loop the animation has finished.
         */
        float getCurrentTime() const { return currentTime; }

    private:
        std::vector<SpriteKeyframe> keyframes;
        float animationDuration;
        bool clampDelta;
        bool loop;

        std::vector<std::vector<SpriteKeyframe>::iterator> frames;
        float frameTime;
        float currentTime = 0;
    };
}
#endif //XENGINE_SPRITEANIMATION_HPP
