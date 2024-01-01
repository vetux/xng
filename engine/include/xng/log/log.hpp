/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_LOG_HPP
#define XENGINE_LOG_HPP

#include <string>
#include <vector>
#include <functional>
#include <set>
#include <mutex>

#include "loglevel.hpp"

namespace xng {
    class Log {
    public:
        class Listener {
        public:
            virtual void initialize(const std::vector<std::pair<LogLevel, std::string>> &logs) = 0;

            virtual void log(LogLevel level, const std::string &message) = 0;
        };

        XENGINE_EXPORT static Log &instance();

        void log(LogLevel level, const std::string &message) {
            std::lock_guard<std::mutex> guard(mutex);
            logs.emplace_back(std::make_pair<>(level, message));
            for (auto &listener: listeners) {
                listener->log(level, message);
            }
        }

        void addListener(Listener *listener) {
            std::lock_guard<std::mutex> guard(mutex);
            listeners.insert(listener);
            listener->initialize(logs);
        }

        void removeListener(Listener *listener) {
            std::lock_guard<std::mutex> guard(mutex);
            listeners.erase(listener);
        }

        std::vector<std::pair<LogLevel, std::string>> getLogs() {
            std::lock_guard<std::mutex> guard(mutex);
            return logs;
        }

    private:
        std::mutex mutex;
        std::set<Listener *> listeners;
        std::vector<std::pair<LogLevel, std::string>> logs;
    };
}

#endif //XENGINE_LOG_HPP
