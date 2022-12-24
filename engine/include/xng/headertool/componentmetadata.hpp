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

#ifndef XENGINE_COMPONENTMETADATA_HPP
#define XENGINE_COMPONENTMETADATA_HPP

#include <numeric>
#include <vector>
#include <string>
#include <map>

#include "xng/headertool/token.hpp"

namespace xng {
    struct ComponentMetadata {
        struct TypeMetadata {
            std::string typeName;
            std::vector<TypeMetadata> templateArguments; // The list of template arguments

            std::string fullTypeName() const {
                return typeName + combineArguments();
            }

            std::string combineArguments() const {
                std::string ret;
                if (!templateArguments.empty()){
                    ret += "<";
                    for (auto &arg: templateArguments) {
                        ret += arg.fullTypeName() + ",";
                    }
                    if (!ret.empty())
                        ret.pop_back();
                    ret += ">";
                }
                return ret;
            }
        };

        struct MemberMetadata {
            TypeMetadata typeMetadata;
            std::string instanceName;
            std::string defaultValue;
            Token minimum;
            Token maximum;
        };

        std::string typeName;
        std::vector<MemberMetadata> members;
        std::string category;
    };
}
#endif //XENGINE_COMPONENTMETADATA_HPP
