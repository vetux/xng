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

#ifndef XENGINE_REFCOUNTER_HPP
#define XENGINE_REFCOUNTER_HPP

#include <map>
#include <limits>
#include <stdexcept>
#include <mutex>

namespace xng {
    template<typename K, typename C>
    class RefCounter {
    public:
        bool inc(K key) {
            std::lock_guard<std::mutex> g(mutex);
            auto &c = counters[key];
            if (c == std::numeric_limits<C>::max()) {
                throw std::runtime_error("Counter overflow");
            }
            bool ret = c == std::numeric_limits<C>::min();
            c++;
            return ret;
        }

        bool dec(K key) {
            std::lock_guard<std::mutex> g(mutex);
            auto &c = counters.at(key);
            if (c == std::numeric_limits<C>::min()) {
                throw std::runtime_error("Counter underflow");
            }
            c--;
            bool ret = c == std::numeric_limits<C>::min();
            if (ret) {
                counters.erase(key);
            }
            return ret;
        }

        C get(K key) {
            std::lock_guard<std::mutex> g(mutex);
            return counters.at(key);
        }

    private:
        std::mutex mutex;
        std::map<K, C> counters;
    };
}
#endif //XENGINE_REFCOUNTER_HPP
