/**
 *  Mana - 3D Game Engine
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

#ifndef MANA_ASSET_HPP
#define MANA_ASSET_HPP

namespace engine {
    class MANA_EXPORT AssetBase {
    public:
        virtual ~AssetBase() = default;

        virtual AssetBase *clone() = 0;
    };

    template<typename T>
    class MANA_EXPORT Asset : public AssetBase {
    public:
        Asset() = default;

        explicit Asset(const T &instance) : instance(instance) {}

        AssetBase *clone() override {
            return new Asset<T>(instance);
        }

        T instance;
    };
}

#endif //MANA_ASSET_HPP
