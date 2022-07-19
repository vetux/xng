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
        animationDuration = (1.0f / numeric_cast<float>(animationFps)) * numeric_cast<float>(totalFrames);
        frameTime = animationDuration / numeric_cast<float>(keyframes.size());
        initFrames();
    }

    const ResourceHandle<Sprite> &SpriteAnimation::getFrame(DeltaTime deltaTime) {
        if (frames.empty())
            throw std::runtime_error("Animation not initialized");

        if (!loop || time < animationDuration) {
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
        }

        auto frame = numeric_cast<size_t>(time / frameTime);
        assert(frame < frames.size());
        return frames.at(frame)->sprite;
    }

    Messageable &SpriteAnimation::operator<<(const Message &message) {
        auto vec = message.value("keyframes");
        if (vec.getType() == Message::LIST) {
            for (auto &kf: vec.asList()) {
                SpriteKeyframe keyframe;
                keyframe << kf;
                keyframes.emplace_back(keyframe);
            }
        }
        animationDuration = message.value("animationDuration", 0.0f);
        clampDelta = message.value("clampDelta", false);
        loop = message.value("loop", true);

        initFrames();
        frameTime = animationDuration / frames.size();
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
                frames.emplace_back(keyframes.begin() + i);
            }
        }
    }

    std::unique_ptr<Resource> SpriteAnimation::clone() {
        return std::make_unique<SpriteAnimation>(*this);
    }

    std::type_index SpriteAnimation::getTypeIndex() {
        return typeid(SpriteAnimation);
    }
}