/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_EVENTBUS_HPP
#define XENGINE_EVENTBUS_HPP

#include <functional>
#include <set>

#include "xng/event/eventlistener.hpp"
#include "xng/util/downcast.hpp"

namespace xng {
    class XENGINE_EXPORT EventBus final {
    public:
        template<typename T>
        void invoke(const T &event) const {
            for (auto &listener: listeners.at(T::typeName)) {
                down_cast<EventListener<T> &>(listener.get()).onEvent(event);
            }
        }

        template<typename T>
        void addListener(EventListener<T> &listener) {
            listeners[T::typeName].insert(listener);
        }

        template<typename T>
        void removeListener(EventListener<T> &listener) {
            listeners[T::typeName].erase(listener);
        }

    private:
        struct CompareRefs {
            bool operator()(const std::reference_wrapper<EventListenerBase> &a,
                            const std::reference_wrapper<EventListenerBase> &b) const {
                return &a.get() < &b.get();
            }
        };

        std::map<std::string, std::set<std::reference_wrapper<EventListenerBase>, CompareRefs> > listeners;
    };
}

#endif //XENGINE_EVENTBUS_HPP
