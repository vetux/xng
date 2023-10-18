/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/animation/sprite/spriteanimation.hpp"
#include "xng/types/time.hpp"

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
        frameTime = animationDuration / static_cast<float>(keyframes.size());
        initFrames();
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
        animationDuration = (1.0f / static_cast<float>(animationFps)) * static_cast<float>(totalFrames);
        frameTime = animationDuration / static_cast<float>(keyframes.size());
        initFrames();
    }

    const ResourceHandle<Sprite> &SpriteAnimation::getFrame(DeltaTime deltaTime) {
        if (frames.empty())
            throw std::runtime_error("Animation not initialized");

        if (clampDelta) {
            if (deltaTime > frameTime) {
                // If deltaTime is larger than frameTime we would skip frames therefore we clamp deltaTime to frameTime
                // which makes the animation advance at most one frame per call to getFrame.
                deltaTime = frameTime;
            }
        }

        time += deltaTime;

        if (time > animationDuration) {
            if (loop) {
                // Set currentTime to difference
                time = time - animationDuration;
            } else {
                time = animationDuration;
            }
        }

        auto frame = static_cast<size_t>(time / frameTime);
        if (frame >= frames.size())
            frame = frames.size() - 1;
        return keyframes.at(frames.at(frame)).sprite;
    }

    Messageable &SpriteAnimation::operator<<(const Message &message) {
        auto vec = message.getMessage("keyframes");
        if (vec.getType() == Message::LIST) {
            for (auto &kf: vec.asList()) {
                SpriteKeyframe keyframe;
                keyframe << kf;
                keyframes.emplace_back(keyframe);
            }
        }
        if (message.has("animationFps")) {
            auto fps = message.getMessage("animationFps", 0);
            int totalFrames = 0;
            for (auto &v: keyframes)
                totalFrames += v.duration;
            animationDuration = (1.0f / static_cast<float>(fps)) * static_cast<float>(totalFrames);
        } else {
            message.value("animationDuration", animationDuration);
        }
        message.value("clampDelta", clampDelta);
        message.value("loop", loop, true);

        initFrames();

        frameTime = animationDuration / static_cast<float>(frames.size());
        time = 0;

        return *this;
    }

    Message &SpriteAnimation::operator>>(Message &message) const {
        message = Message(Message::DICTIONARY);
        auto vec = std::vector<Message>();
        for (auto &kf: keyframes) {
            Message msg;
            kf >> msg;
            vec.emplace_back(msg);
        }
        message["keyframes"] = vec;
        message["animationDuration"] = animationDuration;
        message["clampDelta"] = clampDelta;
        message["loop"] = loop;
        return message;
    }

    void SpriteAnimation::initFrames() {
        frames.clear();
        for (auto i = 0; i < keyframes.size(); i++) {
            auto &keyframe = keyframes.at(i);
            for (int v = 0; v < keyframe.duration; v++) {
                frames.emplace_back(i);
            }
        }
    }

    std::unique_ptr<Resource> SpriteAnimation::clone() {
        return std::make_unique<SpriteAnimation>(*this);
    }

    std::type_index SpriteAnimation::getTypeIndex() const {
        return typeid(SpriteAnimation);
    }
}