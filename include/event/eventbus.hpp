/**
 *  XEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_EVENTBUS_HPP
#define XENGINE_EVENTBUS_HPP

#include <functional>
#include <set>
#include <typeindex>
#include <mutex>

#include "event/eventreceiver.hpp"

namespace xengine {
    class XENGINE_EXPORT EventBus {
    public:
        template<typename T>
        void invoke(const T &event) {
            std::lock_guard<std::mutex> guard(mutex);
            auto id = std::type_index(typeid(T));
            for (auto *receiver: receivers[id]) {
                dynamic_cast<EventReceiver<T> &>(*receiver).onEvent(event);
            }
        }

        template<typename T>
        void subscribe(EventReceiver<T> *receiver) {
            std::lock_guard<std::mutex> guard(mutex);
            receivers[typeid(T)].insert(receiver);
        }

        template<typename T>
        void unsubscribe(EventReceiver<T> *receiver) {
            std::lock_guard<std::mutex> guard(mutex);
            receivers[typeid(T)].erase(receiver);
        }

    private:
        std::mutex mutex;
        std::map<std::type_index, std::set<EventReceiverBase *>> receivers;
    };
}

#endif //XENGINE_EVENTBUS_HPP
