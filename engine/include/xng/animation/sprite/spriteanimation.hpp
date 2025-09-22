/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "spritekeyframe.hpp"

#include "xng/util/time.hpp"

namespace xng {
    /**
     * A sprite animation is a sequence of frames which are displayed sequentially to create the illusion of an animation.
     */
    class XENGINE_EXPORT SpriteAnimation final : public Resource, public Messageable {
    public:
        RESOURCE_TYPENAME(SpriteAnimation)

        SpriteAnimation() = default;

        /**
         * @param frames The keyframes to use in order.
         * @param animationDuration The duration in seconds in which all the frames (Including duration frames of each key frame) should be displayed,
         * where every frame gets the same amount of display time. If clampDelta is true the animation may take more time but never less.
         * @param loop If true the animation will repeat.
         * @param clampDelta If true the animation will clamp deltaTime to frame time,
         * causing the animation to be advanced at most one frame per call to getFrame.
         * This causes animations to slow down when the framerate drops below the animation framerate.
         * The user can verify that an animation has finished by calling getTime()
         */
        SpriteAnimation(std::vector<SpriteKeyframe> frames,
                        float animationDuration,
                        bool loop = true,
                        bool clampDelta = false);

        SpriteAnimation(std::vector<SpriteKeyframe> frames,
                        int animationFps,
                        bool loop = true,
                        bool clampDelta = false);

        std::unique_ptr<Resource> clone() override;

        /**
         * Retrieve the current frame of the animation.
         *
         * @param deltaTime The passed time in seconds. If 0 no change of frame is made.
         * @return The texture corresponding to the current animation frame.
         */
        const ResourceHandle<Sprite> &getFrame(DeltaTime deltaTime = {});

        const std::vector<SpriteKeyframe> &getKeyframes() { return keyframes; }

        /**
         * @return The duration of the animation
         */
        float getDuration() const { return animationDuration; }

        void setDuration(float duration) {
            animationDuration = duration;
            frameTime = animationDuration / static_cast<float>(keyframes.size()) / animationSpeed;
        }

        float getAnimationSpeed() { return animationSpeed; }

        void setAnimationSpeed(const float speed) {
            animationSpeed = speed;
            frameTime = animationSpeed / static_cast<float>(keyframes.size()) / animationSpeed;
        }

        bool isClampingDelta() const { return clampDelta; }

        bool isLooping() const { return loop; }

        /**
         * @return duration / frames.size()
         */
        float getFrameTime() const { return frameTime; }

        /**
         * @return The current total animation time. Never larger than animationDuration, If currentTime == animationDuration and !loop the animation has finished.
         */
        float getTime() const { return time; }

        Messageable &operator<<(const Message &message) override;

        Message &operator>>(Message &message) const override;

    private:
        void initFrames();

        std::vector<SpriteKeyframe> keyframes;
        float animationDuration{};
        bool clampDelta{};
        bool loop{};
        float animationSpeed = 1;

        std::vector<size_t> frames; // Each frame of the animation
        float frameTime{}; // The time in seconds that one frame should be visible for
        float time = 0; // The currently accumulated time
    };
}
#endif //XENGINE_SPRITEANIMATION_HPP
