/**
 *  xEngine - C++ game engine library
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

#ifndef XENGINE_FRAMEGRAPHBLACKBOARD_HPP
#define XENGINE_FRAMEGRAPHBLACKBOARD_HPP

#include <map>
#include <typeindex>
#include <memory>

namespace xengine {
    class XENGINE_EXPORT FrameGraphBlackboard {
    public:
        template<typename T>
        void set(const T &instance) {
            data[typeid(T)] = std::make_unique<Entry < T>>
            (instance);
        }

        template<typename T>
        const T &get() {
            if (data.find(typeid(T)) == data.end()) {
                data[typeid(T)] = std::make_unique<Entry<T>>(T());
            }
            return dynamic_cast<Entry <T> &>(*data[typeid(T)].get()).value;
        }

        void clear() {
            data.clear();
        }

    private:
        struct EntryBase {
            virtual ~EntryBase() = default;
        };

        template<typename T>
        struct Entry : public EntryBase {
            explicit Entry(T value)
                    : value(std::move(value)) {}

            ~Entry() override = default;

            T value;
        };

        std::map<std::type_index, std::unique_ptr<EntryBase>> data;
    };
}

#endif //XENGINE_FRAMEGRAPHBLACKBOARD_HPP
