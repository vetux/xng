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

#include <algorithm>
#include <filesystem>

#include "xng/ecs/systems/meshrendersystem.hpp"
#include "xng/ecs/components.hpp"
#include "xng/util/time.hpp"

//TODO: Reimplement MeshRenderSystem
namespace xng {
    MeshRenderSystem::MeshRenderSystem()
    {
    }

    MeshRenderSystem::~MeshRenderSystem() = default;

    void MeshRenderSystem::start(EntityScene &entityManager, EventBus &eventBus) {}

    void MeshRenderSystem::stop(EntityScene &entityManager, EventBus &eventBus) {}

    void MeshRenderSystem::update(DeltaTime deltaTime, EntityScene &entScene, EventBus &eventBus) {
      /*  Scene scene = {};

        // Get objects
        for (auto &pair: entScene.getPool<SkinnedMeshComponent>()) {
            // TODO: Culling
            // TODO: Z Sort transparent meshes based on distance of transform to camera
            // TODO: Change transform walking / scene creation to allow model matrix caching

            auto &transform = entScene.getComponent<TransformComponent>(pair.first);
            if (!transform.enabled)
                continue;

            auto &meshComponent = pair.second;
            if (!meshComponent.enabled)
                continue;

            Node node;

            TransformProperty transformProperty;
            transformProperty.transform = TransformComponent::walkHierarchy(transform, entScene);
            node.addProperty(transformProperty);

            SkinnedMeshProperty meshProperty;
            meshProperty.mesh = pair.second.mesh;
            node.addProperty(meshProperty);

            ShadowProperty shadowProperty;
            shadowProperty.castShadows = pair.second.castShadows;
            shadowProperty.receiveShadows = pair.second.receiveShadows;
            node.addProperty(shadowProperty);

            if (entScene.checkComponent<MaterialComponent>(pair.first)) {
                auto &comp = entScene.getComponent<MaterialComponent>(pair.first);
                MaterialProperty materialProperty;
                materialProperty.materials = comp.materials;
                node.addProperty(materialProperty);
            }

            if (entScene.checkComponent<RigAnimationComponent>(pair.first)) {
                BoneTransformsProperty boneTransformsProperty;
                boneTransformsProperty.boneTransforms = entScene.getComponent<RigAnimationComponent>(
                        pair.first).boneTransforms;
                node.addProperty(boneTransformsProperty);
            }

            scene.rootNode.childNodes.emplace_back(node);
        }

        // Get skybox
        for (auto &pair: entScene.getPool<SkyboxComponent>()) {
            auto &comp = pair.second;
            auto boxProp = SkyboxProperty();
            boxProp.skybox = comp.skybox;
            scene.rootNode.addProperty(boxProp);
        }

        // Get Camera
        for (auto &pair: entScene.getPool<CameraComponent>()) {
            auto &tcomp = entScene.getComponent<TransformComponent>(pair.first);

            if (!tcomp.enabled)
                continue;

            auto &comp = pair.second;

            Node cameraNode;

            auto camProp = CameraProperty();
            camProp.camera = comp.camera;
            cameraNode.addProperty(camProp);

            auto tProp = TransformProperty();
            tProp.transform = TransformComponent::walkHierarchy(tcomp, entScene);
            cameraNode.addProperty(tProp);

            scene.rootNode.childNodes.emplace_back(cameraNode);

            break;
        }

        // Get lights

        for (auto &pair: entScene.getPool<LightComponent>()) {
            auto lightComponent = pair.second;
            auto &tcomp = entScene.getPool<TransformComponent>().lookup(pair.first);

            if (!lightComponent.enabled)
                continue;

            if (!tcomp.enabled)
                continue;


            Node node;

            TransformProperty transformProperty;
            transformProperty.transform = tcomp.transform;
            node.addProperty(transformProperty);

            switch (lightComponent.light.index()) {
                case 0: {
                    auto tmp = std::get<PointLight>(lightComponent.light);
                    PointLightProperty prop;
                    prop.light = tmp;
                    node.addProperty(prop);
                    break;
                }
                case 1: {
                    auto tmp = std::get<DirectionalLight>(lightComponent.light);
                    DirectionalLightProperty prop;
                    prop.light = tmp;
                    node.addProperty(prop);
                    break;
                }
                case 2: {
                    auto tmp = std::get<SpotLight>(lightComponent.light);
                    SpotLightProperty prop;
                    prop.light = tmp;
                    node.addProperty(prop);
                    break;
                }
            }
            scene.rootNode.childNodes.emplace_back(node);
        }

        // Render
        renderer.render(scene);*/
    }
}