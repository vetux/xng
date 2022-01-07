/**
 *  XEngine - C++ game engine library
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

#include <algorithm>
#include <memory>
#include <sstream>

#include "ecs/systems/monoscriptingsystem.hpp"
#include "ecs/components.hpp"

#include "schema/ecsschema.hpp"
#include "io/protocol/jsonprotocol.hpp"

namespace xengine {
    /*struct RuntimeScript : Component::UserData {
        bool enabled = false;
        std::unique_ptr<Script> script = nullptr; //TODO: OnDisable is not called when the component is destroyed.
    };

    void uploadScene(MonoCppDomain &runtime,
                     MonoCppAssembly &msCorLib,
                     MonoCppAssembly &manaAssembly,
                     Scene &scene) {
        Scene upload;
        for (auto &n : scene.nodes) {
            if (n.second.hasComponent<MonoSyncComponent>())
                upload.nodes.insert(n);
        }

        std::stringstream stream;

        Message message;
        JsonProtocol().serialize(stream, message << upload);

        std::string s = stream.str();

        MonoCppObject str = runtime.stringFromUtf8(s);

        MonoCppArguments args;
        args.add(str);

        //This call causes a memory leak, most likely in the csharp json deserializer.
        manaAssembly.invokeStaticMethod("Mana.Internal", "SceneInterface", "setSceneJson", args);
    }

    void downloadScene(MonoCppDomain &domain,
                       MonoCppAssembly &msCorLib,
                       MonoCppAssembly &manaAssembly,
                       Scene &scene) {
        auto str = manaAssembly.invokeStaticMethod("Mana.Internal", "SceneInterface", "getSceneJson");

        std::stringstream stream(domain.stringToUtf8(str));
        Scene monoScene;
        auto message = JsonProtocol().deserialize(stream);

        monoScene << message;

        //Synchronize transforms
        for (auto &node : monoScene.nodes) {
            if (!node.second.hasComponent<TransformComponent>())
                continue;
            auto &comp = node.second.getComponent<TransformComponent>();
            scene.nodes.at(node.first).getComponent<TransformComponent>().transform = comp.transform;
            scene.nodes.at(node.first).getComponent<TransformComponent>().parent = comp.parent;
        }
    }*/

    MonoScriptingSystem::MonoScriptingSystem(Input &input,
                                             MonoCppDomain &domain,
                                             MonoCppAssembly &manaAssembly,
                                             Archive &archive)
            : domain(domain),
              msCorLib(domain.getMsCorLibAssembly()),
              manaAssembly(manaAssembly),
              input(input),
              archive(archive) {}

    void MonoScriptingSystem::start(EntityManager &entityManager) {
        input.addListener(*this);
    }

    void MonoScriptingSystem::stop(EntityManager &entityManager) {
        input.removeListener(*this);
    }

    void MonoScriptingSystem::update(float deltaTime, EntityManager &manager) {
        /*manaAssembly.setStaticField("Mana", "Time", "deltaTime", &deltaTime);

        uploadScene(domain, msCorLib, manaAssembly, scene);

        auto nodes = scene.findNodesWithComponent<MonoScriptComponent>();

        std::sort(nodes.begin(), nodes.end(),
                  [](const Node *a, const Node *b) -> bool {
                      return a->getComponent<MonoScriptComponent>().queue <
                             b->getComponent<MonoScriptComponent>().queue;
                  });

        std::set<std::string> usedAssemblies;

        for (auto *node : nodes) {
            auto &comp = node->getComponent<MonoScriptComponent>();

            if (comp.userData == nullptr) {
                comp.userData = std::make_unique<RuntimeScript>();
            }

            auto &data = *dynamic_cast<RuntimeScript *>(comp.userData.get());

            usedAssemblies.insert(comp.assembly);

            if (data.script == nullptr) {
                auto &assembly = getAssembly(comp.assembly);
                data.script = std::make_unique<MonoScript>(&assembly, comp.nameSpace, comp.className);
                data.enabled = false;
            }

            if (!node->enabled || !comp.enabled) {
                if (data.enabled) {
                    data.enabled = false;
                    data.script->onDisable();
                }
            }

            if (!data.enabled) {
                data.enabled = true;
                data.script->onEnable();
            }

            data.script->onUpdate();
        }

        std::set<std::string> unused;
        for (auto &pair : assemblies) {
            if (usedAssemblies.find(pair.first) == usedAssemblies.end())
                unused.insert(pair.first);
        }
        for (auto &s : unused) {
            assemblies.erase(s);
        }

        downloadScene(domain, msCorLib, manaAssembly, scene);
        manaAssembly.invokeStaticMethod("Mana", "Input", "OnFrameEnd");*/
    }

 /*  void MonoScriptingSystem::onKeyDown(Key key) {
        MonoCppArguments args;
        int v = key;
        args.add(v);
        manaAssembly.invokeStaticMethod("Mana", "Input", "OnKeyDown", args);
    }

    void MonoScriptingSystem::onKeyUp(Key key) {
        MonoCppArguments args;
        int v = key;
        args.add(v);
        manaAssembly.invokeStaticMethod("Mana", "Input", "OnKeyUp", args);
    }

    void MonoScriptingSystem::onMouseMove(double xpos, double ypos) {
        MonoCppArguments args;
        args.add(xpos);
        args.add(ypos);
        manaAssembly.invokeStaticMethod("Mana", "Input", "OnMouseMove", args);
    }

    void MonoScriptingSystem::onMouseWheelScroll(double ammount) {
        MonoCppArguments args;
        args.add(ammount);
        manaAssembly.invokeStaticMethod("Mana", "Input", "OnMouseWheelScroll", args);
    }

    void MonoScriptingSystem::onMouseKeyDown(MouseKey key) {
        int v = key;
        MonoCppArguments args;
        args.add(v);
        manaAssembly.invokeStaticMethod("Mana", "Input", "OnMouseKeyDown", args);
    }

    void MonoScriptingSystem::onMouseKeyUp(MouseKey key) {
        int v = key;
        MonoCppArguments args;
        args.add(v);
        manaAssembly.invokeStaticMethod("Mana", "Input", "OnMouseKeyUp", args);
    }

    void MonoScriptingSystem::onTextInput(const std::string &text) {
        MonoCppArguments args;
        auto monostr = domain.stringFromUtf8(text);
        args.add(monostr);
        manaAssembly.invokeStaticMethod("Mana", "Input", "OnTextInput", args);
    }

    MonoCppAssembly &MonoScriptingSystem::getAssembly(const std::string &path) {
        if (assemblies.find(path) == assemblies.end()) {
            std::unique_ptr<std::iostream> stream(archive.open(path));
            assemblies[path] = domain.loadAssembly(*stream);
        }
        return *assemblies[path];
    }*/
}