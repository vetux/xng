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

#ifndef XENGINE_ECSSAMPLE_HPP
#define XENGINE_ECSSAMPLE_HPP

#include <string>

#include "io/messageable.hpp"

namespace xng {
    struct ECSSample : public Messageable {
        std::string systemName; // The name of the system
        long time; // The time the system update has taken to complete in milliseconds

        Messageable &operator<<(const Message &message) override {
            systemName = message.value("systemName", std::string());
            time = message.value("time", 0);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["systemName"] = systemName;
            message["time"] = time;
            return message;
        }
    };
}

#endif //XENGINE_ECSSAMPLE_HPP
