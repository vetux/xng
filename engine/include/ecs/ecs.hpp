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

#ifndef XENGINE_ECS_HPP
#define XENGINE_ECS_HPP

#include <set>

#include "ecs/system.hpp"

namespace xng {
    class XENGINE_EXPORT ECS {
    public:
        explicit ECS(std::vector<System *> systems = {});

        ~ECS();

        ECS(ECS &&other) noexcept;

        ECS &operator=(ECS &&other) noexcept;

        void start();

        void update(float deltaTime);

        void stop();

        EntityManager &getEntityManager();

    private:
        EntityManager entityManager;
        std::vector<System *> systems;
    };
}

#endif //XENGINE_ECS_HPP
