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

#ifndef XENGINE_COUNTER_HPP
#define XENGINE_COUNTER_HPP

#include <limits>
#include <stack>
#include <stdexcept>
#include <mutex>

namespace xng {
    template<typename T>
    class Counter {
    public:
        T get() {
            std::lock_guard<std::mutex> g(mutex);
            if (returned.empty()) {
                if (counter == std::numeric_limits<T>::max()) {
                    throw std::runtime_error("Counter overflow");
                }
                return counter++;
            } else {
                auto ret = returned.top();
                returned.pop();
                return ret;
            }
        }

        void put(T v) {
            std::lock_guard<std::mutex> g(mutex);
            returned.push(v);
        }

    private:
        std::mutex mutex;
        T counter = std::numeric_limits<T>::min();
        std::stack<T> returned;
    };
}
#endif //XENGINE_COUNTER_HPP
