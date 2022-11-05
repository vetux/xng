/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_ECSFRAMELIST_HPP
#define XENGINE_ECSFRAMELIST_HPP

#include "xng/io/messageable.hpp"
#include "ecsframe.hpp"

namespace xng {
    struct ECSFrameList : public Messageable {
        std::vector<ECSFrame> frames;

        void addFrame(const ECSFrame &frame) {
            frames.emplace_back(frame);
        }

        Messageable &operator<<(const Message &message) override {
            frames.clear();
            for (auto &msg: message.asList()) {
                ECSFrame frame;
                frame << msg;
                frames.emplace_back(frame);
            }
            return *this;
        }

        Message &operator>>(Message &message) const override {
            auto vec = std::vector<Message>();
            for (auto &frame: frames) {
                Message msg;
                frame >> msg;
                vec.emplace_back(msg);
            }
            message = vec;
            return message;
        }
    };
}
#endif //XENGINE_ECSFRAMELIST_HPP
