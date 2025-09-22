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

#ifndef XENGINE_TIME_HPP
#define XENGINE_TIME_HPP

#include <chrono>

#include "xng/io/messageable.hpp"

namespace xng {
    class Duration : public Messageable {
    public:
        Duration() = default;

        explicit Duration(std::chrono::nanoseconds nanoseconds)
                : nanoseconds(nanoseconds),
                  seconds(static_cast<double>(nanoseconds.count()) *
                          (1.0f / static_cast<double>(std::chrono::nanoseconds::period::den))) {}

        explicit Duration(double seconds)
                : nanoseconds(static_cast<std::chrono::nanoseconds::rep>(seconds *
                                                                         static_cast<double>(std::chrono::nanoseconds::period::den))),
                  seconds(seconds) {}

        operator double() const {
            return seconds;
        }

        bool operator>(const Duration &other) const {
            return nanoseconds > other.nanoseconds;
        }

        bool operator<(const Duration &other) const {
            return nanoseconds < other.nanoseconds;
        }

        bool operator==(const Duration &other) const {
            return nanoseconds == other.nanoseconds;
        }

        Duration &operator=(const double &value) {
            seconds = value;
            nanoseconds = std::chrono::nanoseconds(static_cast<std::chrono::nanoseconds::rep>(seconds *
                                                                                              static_cast<double>(std::chrono::nanoseconds::period::den)));
            return *this;
        }

        Duration &operator+=(const double &value) {
            seconds += value;
            nanoseconds = std::chrono::nanoseconds(static_cast<std::chrono::nanoseconds::rep>(seconds *
                                                                                              static_cast<double>(std::chrono::nanoseconds::period::den)));
            return *this;
        }

        Duration &operator-=(const double &value) {
            seconds -= value;
            nanoseconds = std::chrono::nanoseconds(static_cast<std::chrono::nanoseconds::rep>(seconds *
                                                                                              static_cast<double>(std::chrono::nanoseconds::period::den)));
            return *this;
        }

        Duration &operator*=(const double &value) {
            seconds *= value;
            nanoseconds = std::chrono::nanoseconds(static_cast<std::chrono::nanoseconds::rep>(seconds *
                                                                                              static_cast<double>(std::chrono::nanoseconds::period::den)));
            return *this;
        }

        Duration &operator/=(const double &value) {
            seconds /= value;
            nanoseconds = std::chrono::nanoseconds(static_cast<std::chrono::nanoseconds::rep>(seconds *
                                                                                              static_cast<double>(std::chrono::nanoseconds::period::den)));
            return *this;
        }

        void add(const Duration &duration) {
            nanoseconds += duration.nanoseconds;
        }

        Duration difference(const Duration &other) const {
            if (*this > other) {
                return Duration(nanoseconds - other.nanoseconds);
            } else {
                return Duration(other.nanoseconds - nanoseconds);
            }
        }

        Messageable &operator<<(const Message &message) override {
            nanoseconds = std::chrono::nanoseconds(message.asULongLong());
            seconds = static_cast<double>(nanoseconds.count()) *
                      (1.0f / static_cast<double>(std::chrono::nanoseconds::period::den));
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(static_cast<unsigned long long>(nanoseconds.count()));
            return message;
        }

        std::chrono::nanoseconds nanoseconds{};
        double seconds{};
    };

    typedef Duration DeltaTime;
}

#endif //XENGINE_TIME_HPP
