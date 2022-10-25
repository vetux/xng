/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_RIG_HPP
#define XENGINE_RIG_HPP

#include <map>
#include <utility>
#include <vector>
#include <string>

#include "animation/skeletal/bone.hpp"

namespace xng {
    class XENGINE_EXPORT Rig {
    public:
        Rig() = default;

        explicit Rig(std::vector<Bone> bones)
                : bones(std::move(bones)) {
            for (auto i = 0; i < this->bones.size(); i++) {
                auto &bone = this->bones.at(i);
                boneNameMapping[bone.name] = this->bones.begin() + i;
            }
        }

        const std::vector<Bone> &getBones() const { return bones; }

        Bone &getBone(size_t index) { return bones.at(index); }

        Bone &getBone(const std::string &name) { return *boneNameMapping.at(name); }

        operator bool() const {
            return !bones.empty();
        }

    private:
        std::vector<Bone> bones;
        std::map<std::string, std::vector<Bone>::iterator> boneNameMapping;
    };
}

#endif //XENGINE_RIG_HPP
