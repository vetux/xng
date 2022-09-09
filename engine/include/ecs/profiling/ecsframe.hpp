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

#ifndef XENGINE_ECSFRAME_HPP
#define XENGINE_ECSFRAME_HPP

#include "ecs/profiling/ecssample.hpp"
#include "io/messageable.hpp"

namespace xng {
    struct ECSFrame : public Messageable {
        long duration; // Total duration of the frame in milliseconds
        std::vector<ECSSample> samples;

        Messageable &operator<<(const Message &message) override {
            samples.clear();
            duration = message.value("duration", 0);
            for (auto &msg: message["samples"].asList()) {
                ECSSample sample;
                sample << msg;
                samples.emplace_back(sample);
            }
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["duration"] = duration;

            auto vec = std::vector<Message>();
            for (auto &sample: samples) {
                Message msg;
                sample >> msg;
                vec.emplace_back(msg);
            }
            message["samples"] = vec;

            return message;
        }
    };
}
#endif //XENGINE_ECSFRAME_HPP
