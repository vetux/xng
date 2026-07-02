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

#ifndef XENGINE_RENDERPASSREGISTRY_HPP
#define XENGINE_RENDERPASSREGISTRY_HPP

#include <unordered_map>
#include <variant>

#include "xng/rendergraph/resourceid.hpp"
#include "xng/rendergraph/resource/buffer.hpp"
#include "xng/rendergraph/resource/texture.hpp"

namespace xng {
    /**
     * Contains resources to be shared between passes.
     */
    class RenderPassRegistry {
    public:
        enum ResourceName {
            G_BUFFER_POSITION,
            G_BUFFER_NORMAL,
            G_BUFFER_TANGENT,
            G_BUFFER_ROUGHNESS_METALLIC_AO,
            G_BUFFER_ALBEDO,
            G_BUFFER_OBJECT_ID_RECEIVE_SHADOWS,
            G_BUFFER_DEPTH,

            PBR_COLOR_DEFERRED,
            PBR_COLOR_FORWARD,

            CANVAS_COLOR,

            RESOURCE_USER = 255, // User-defined resource names should start at RESOURCE_USER
        };

        void set(const ResourceName name, const rg::Resource<rg::Texture> &texture) {
            resources[name] = texture;
        }

        void set(const ResourceName name, const rg::Resource<rg::Buffer>& buffer) {
            resources[name] = buffer;
        }

        const rg::Resource<rg::Texture> &getTexture(const ResourceName name) const {
            const auto it = resources.find(name);
            if (it == resources.end()) {
                throw std::runtime_error("Resource not found");
            }
            if (it->second.index() != 0) {
                throw std::runtime_error("Resource is not a texture");
            }
            return std::get<rg::Resource<rg::Texture>>(it->second);
        }

        const rg::Resource<rg::Buffer> &getBuffer(const ResourceName name) const {
            const auto it = resources.find(name);
            if (it == resources.end()) {
                throw std::runtime_error("Resource not found");
            }
            if (it->second.index() != 1) {
                throw std::runtime_error("Resource is not a buffer");
            }
            return std::get<rg::Resource<rg::Buffer>>(it->second);
        }

    private:
        std::unordered_map<ResourceName, std::variant<rg::Resource<rg::Texture>, rg::Resource<rg::Buffer>>> resources;
    };
}

#endif //XENGINE_RENDERPASSREGISTRY_HPP
