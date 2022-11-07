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

#include <sstream>

#include "xng/resource/parsers/jsonparser.hpp"

#include "xng/async/threadpool.hpp"

#include "xng/asset/texture.hpp"
#include "xng/asset/material.hpp"
#include "xng/animation/sprite/spriteanimation.hpp"
#include "xng/ecs/entityscene.hpp"

#include "xng/physics/colliderdesc.hpp"

#include "xng/io/protocol/jsonprotocol.hpp"

#include "xng/resource/resourceimporter.hpp"

namespace xng {
    static void loadBundle(const std::string &bundlePath,
                           ThreadPool &pool,
                           Archive &archive,
                           const ResourceImporter &importer,
                           std::mutex &bundleMutex,
                           std::map<std::string, std::shared_ptr<Task>> bundleTasks,
                           std::map<std::string, ResourceBundle> refBundles) {
        bundleMutex.lock();

        auto bundleIterator = refBundles.find(bundlePath);

        if (bundleIterator == refBundles.end()) {
            auto taskIterator = bundleTasks.find(bundlePath);
            if (taskIterator == bundleTasks.end()) {
                bundleTasks[bundlePath] = pool.addTask(
                        [&archive, &importer, &bundleMutex, &refBundles, &bundlePath, &pool]() {
                            std::filesystem::path path(bundlePath);

                            std::unique_ptr<std::istream> stream(archive.open(bundlePath));
                            auto bundle = importer.import(*stream, path.extension().string(), &archive);

                            std::lock_guard<std::mutex> guard(bundleMutex);
                            refBundles[bundlePath] = std::move(bundle);
                        });
            }
        }

        bundleMutex.unlock();
    }

    Message JsonParser::createBundle(const std::map<std::string, std::reference_wrapper<Resource>> &resources) {
        auto materials = std::vector<Message>();
        auto textures = std::vector<Message>();
        auto sprites = std::vector<Message>();
        auto colliders = std::vector<Message>();
        auto animations = std::vector<Message>();
        auto scenes = std::vector<Message>();

        for (auto &pair: resources) {
            auto msg = Message(Message::DICTIONARY);
            msg["name"] = pair.first;
            auto type = pair.second.get().getTypeIndex();
            if (type == typeid(Material)) {
                auto &res = dynamic_cast<Material &>(pair.second.get());
                res >> msg;
                materials.emplace_back(msg);
            } else if (type == typeid(Texture)) {
                auto &res = dynamic_cast<Texture &>(pair.second.get());
                res >> msg;
                textures.emplace_back(msg);
            } else if (type == typeid(Sprite)) {
                auto &res = dynamic_cast<Sprite &>(pair.second.get());
                res >> msg;
                sprites.emplace_back(msg);
            } else if (type == typeid(ColliderDesc)) {
                auto &res = dynamic_cast<ColliderDesc &>(pair.second.get());
                res >> msg;
                colliders.emplace_back(msg);
            } else if (type == typeid(SpriteAnimation)) {
                auto &res = dynamic_cast<SpriteAnimation &>(pair.second.get());
                res >> msg;
                animations.emplace_back(msg);
            } else if (type == typeid(EntityScene)) {
                auto &res = dynamic_cast<EntityScene &>(pair.second.get());
                res >> msg;
                scenes.emplace_back(msg);
            } else {
                throw std::runtime_error("Unsupported resource type: " + std::string(type.name()));
            }
        }

        Message ret = Message(Message::DICTIONARY);
        if (!materials.empty()) {
            ret["materials"] = materials;
        }
        if (!textures.empty()) {
            ret["textures"] = textures;
        }
        if (!sprites.empty()) {
            ret["sprites"] = sprites;
        }
        if (!colliders.empty()) {
            ret["colliders"] = colliders;
        }
        if (!animations.empty()) {
            ret["sprite-animations"] = animations;
        }
        if (!scenes.empty()) {
            ret["scenes"] = scenes;
        }
        return ret;
    }

    static ResourceBundle readJsonBundle(const std::vector<char> &buffer, const ResourceImporter &importer) {
        auto stream = std::stringstream(std::string(buffer.begin(), buffer.end()));
        const Message m = JsonProtocol().deserialize(stream);

        ResourceBundle ret;

        if (m.has("materials") && m.at("materials").getType() == Message::LIST) {
            for (auto &element: m.at("materials").asList()) {
                std::string name = element.value("name", std::string());
                Material mat;
                mat << element;
                ret.add(name, std::make_unique<Material>(mat));
            }
        }

        if (m.has("textures") && m.at("textures").getType() == Message::LIST) {
            for (auto &element: m.at("textures").asList()) {
                std::string name = element.value("name", std::string());
                Texture tex;
                tex << element;
                ret.add(name, std::make_unique<Texture>(tex));
            }
        }

        if (m.has("sprites") && m.at("sprites").getType() == Message::LIST) {
            for (auto &element: m.at("sprites").asList()) {
                std::string name = element.value("name", std::string());
                Sprite sprite;
                sprite << element;
                ret.add(name, std::make_unique<Sprite>(sprite));
            }
        }

        if (m.has("colliders") && m.at("colliders").getType() == Message::LIST) {
            for (auto &element: m.at("colliders").asList()) {
                std::string name = element.value("name", std::string());
                ColliderDesc desc;
                desc << element;
                ret.add(name, std::make_unique<ColliderDesc>(desc));
            }
        }

        if (m.has("sprite-animations") && m.at("sprite-animations").getType() == Message::LIST) {
            for (auto &element: m.at("sprite-animations").asList()) {
                std::string name = element.value("name", std::string());
                SpriteAnimation animation;
                animation << element;
                ret.add(name, std::make_unique<SpriteAnimation>(animation));
            }
        }

        if (m.has("scenes") && m.at("scenes").getType() == Message::LIST) {
            for (auto &element: m.at("scenes").asList()) {
                std::string name = element.value("name", std::string());
                EntityScene scene;
                scene << element;
                scene.setName(name);
                ret.add(name, std::make_unique<EntityScene>(scene));
            }
        }

        return ret;
    }

    ResourceBundle JsonParser::read(const std::vector<char> &buffer,
                                    const std::string &hint,
                                    const ResourceImporter &importer,
                                    Archive *archive) const {
        return readJsonBundle(buffer, importer);
    }

    const std::set<std::string> &JsonParser::getSupportedFormats() const {
        static const std::set<std::string> formats = {".json", ".bundle"};
        return formats;
    }
}