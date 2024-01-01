/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include <sstream>

#include "xng/resource/importers/jsonimporter.hpp"

#include "xng/async/threadpool.hpp"

#include "xng/render/scene/texture.hpp"
#include "xng/render/scene/material.hpp"
#include "xng/render/scene/cubemap.hpp"
#include "xng/animation/sprite/spriteanimation.hpp"
#include "xng/ecs/entityscene.hpp"

#include "xng/physics/colliderdesc.hpp"

#include "xng/io/protocol/jsonprotocol.hpp"

#include "xng/resource/resourceimporter.hpp"

namespace xng {
    Message JsonImporter::createBundle(const ResourceBundle &bundle) {
        auto materials = std::vector<Message>();
        auto textures = std::vector<Message>();
        auto cubeMaps = std::vector<Message>();
        auto sprites = std::vector<Message>();
        auto animations = std::vector<Message>();
        auto scenes = std::vector<Message>();

        for (auto &pair: bundle.assets) {
            auto msg = Message(Message::DICTIONARY);
            auto type = pair.second.get()->getTypeIndex();
            if (type == typeid(Material)) {
                auto &res = dynamic_cast<Material &>(*pair.second);
                res >> msg;
                msg["name"] = pair.first;
                materials.emplace_back(msg);
            } else if (type == typeid(Texture)) {
                auto &res = dynamic_cast<Texture &>(*pair.second);
                res >> msg;
                msg["name"] = pair.first;
                textures.emplace_back(msg);
            } else if (type == typeid(Sprite)) {
                auto &res = dynamic_cast<Sprite &>(*pair.second);
                res >> msg;
                msg["name"] = pair.first;
                sprites.emplace_back(msg);
            } else if (type == typeid(SpriteAnimation)) {
                auto &res = dynamic_cast<SpriteAnimation &>(*pair.second);
                res >> msg;
                msg["name"] = pair.first;
                animations.emplace_back(msg);
            } else if (type == typeid(CubeMap)) {
                auto &res = dynamic_cast<CubeMap &>(*pair.second);
                res >> msg;
                msg["name"] = pair.first;
                cubeMaps.emplace_back(msg);
            } else {
                throw std::runtime_error("Unsupported resource type: " + std::string(type.name()));
            }
        }

        Message ret = Message(Message::DICTIONARY);
        ret["materials"] = Message(materials);
        ret["textures"] = Message(textures);
        ret["cubeMaps"] = Message(cubeMaps);
        ret["sprites"] = Message(sprites);
        ret["sprite-animations"] = Message(animations);
        return ret;
    }

    static ResourceBundle readJsonBundle(const std::vector<char> &buffer) {
        auto stream = std::stringstream(std::string(buffer.begin(), buffer.end()));
        const Message m = JsonProtocol().deserialize(stream);

        ResourceBundle ret;

        if (m.has("name") || m.has("entities")) {
            // Parse as EntityScene
            auto name = m.getMessage("name", std::string()).asString();
            EntityScene scene;
            scene << m;
            ret.add(name, std::make_unique<EntityScene>(scene));
        } else {
            // Parse as ResourceBundle
            if (m.has("materials") && m.at("materials").getType() == Message::LIST) {
                for (auto &element: m.at("materials").asList()) {
                    auto name = element.getMessage("name", std::string()).asString();
                    Material mat;
                    mat << element;
                    ret.add(name, std::make_unique<Material>(mat));
                }
            }

            if (m.has("textures") && m.at("textures").getType() == Message::LIST) {
                for (auto &element: m.at("textures").asList()) {
                    auto name = element.getMessage("name", std::string()).asString();
                    Texture tex;
                    tex << element;
                    ret.add(name, std::make_unique<Texture>(tex));
                }
            }

            if (m.has("cubeMaps") && m.at("cubeMaps").getType() == Message::LIST) {
                for (auto &element: m.at("cubeMaps").asList()) {
                    auto name = element.getMessage("name", std::string()).asString();
                    CubeMap tex;
                    tex << element;
                    ret.add(name, std::make_unique<CubeMap>(tex));
                }
            }

            if (m.has("sprites") && m.at("sprites").getType() == Message::LIST) {
                for (auto &element: m.at("sprites").asList()) {
                    auto name = element.getMessage("name", std::string()).asString();
                    Sprite sprite;
                    sprite << element;
                    ret.add(name, std::make_unique<Sprite>(sprite));
                }
            }

            if (m.has("sprite-animations") && m.at("sprite-animations").getType() == Message::LIST) {
                for (auto &element: m.at("sprite-animations").asList()) {
                    auto name = element.getMessage("name", std::string()).asString();
                    SpriteAnimation animation;
                    animation << element;
                    ret.add(name, std::make_unique<SpriteAnimation>(animation));
                }
            }
        }
        return ret;
    }

    ResourceBundle JsonImporter::read(std::istream &stream,
                                      const std::string &hint,
                                      const std::string &path,
                                      Archive *archive) {
        std::vector<char> buffer;

        char c;
        while (!stream.eof()) {
            stream.read(&c, 1);
            if (stream.gcount() == 1) {
                buffer.emplace_back(c);
            }
        }

        return readJsonBundle(buffer);
    }

    const std::set<std::string> &JsonImporter::getSupportedFormats() const {
        static const std::set<std::string> formats = {".json"};
        return formats;
    }
}