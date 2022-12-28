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

#ifndef XENGINE_METADATACOMPONENT_HPP
#define XENGINE_METADATACOMPONENT_HPP

#include "xng/headertool/componentmetadata.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    /**
     * This component creates multiple components from metadata entries when serializing.
     * It cannot be deserialized.
     *
     * Used by the editor.
     */
    struct MetadataComponent : public Component {
        struct Entry {
            ComponentMetadata metadata;
            Message value;
        };

        std::vector<Entry> entries;

        std::type_index getType() const override {
            return typeid(MetadataComponent);
        }
    };
}

#endif //XENGINE_METADATACOMPONENT_HPP
