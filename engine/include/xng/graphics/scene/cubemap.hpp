/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_CUBEMAP_HPP
#define XENGINE_CUBEMAP_HPP


#include "xng/resource/resourcehandle.hpp"

#include "xng/graphics/image.hpp"

#include "xng/rendergraph/rendergraphtexture.hpp"

namespace xng {
    struct XENGINE_EXPORT CubeMap final : Resource, Messageable {
        RESOURCE_TYPENAME(CubeMap)

        ~CubeMap() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<CubeMap>(*this);
        }

        Messageable &operator<<(const Message &message) override {
            images.clear();
            for (auto &pair : message.getMessage("images").asDictionary()){
                ResourceHandle<ImageRGBA> image;
                image << pair.second;
                CubeMapFace key;
                key = static_cast<CubeMapFace>(std::stoi(pair.first));
                images[key] = image;
            }
            message.value("description", description);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            Message dict(Message::DICTIONARY);
            for (auto &pair: images) {
                Message value;
                pair.second >> value;
                dict[std::to_string(pair.first)] = value;
            }
            message["images"] = dict;
            description >> message["description"];
            return message;
        }

        std::map<CubeMapFace, ResourceHandle<ImageRGBA>> images;
        RenderGraphTexture description;

        bool isLoaded() const override  {
            for (auto &pair: images){
                if (!pair.second.isLoaded())
                    return false;
            }
            return true;
        }
    };
}

#endif //XENGINE_CUBEMAP_HPP
