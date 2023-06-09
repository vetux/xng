/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

namespace xng {
    MeshRenderSystem::MeshRenderSystem(SceneRenderer &pipeline)
            : pipeline(pipeline) {
    }

    MeshRenderSystem::~MeshRenderSystem() = default;

    void MeshRenderSystem::start(EntityScene &entityManager, EventBus &eventBus) {}

    void MeshRenderSystem::stop(EntityScene &entityManager, EventBus &eventBus) {}

    void MeshRenderSystem::update(DeltaTime deltaTime, EntityScene &entScene, EventBus &eventBus) {
        Scene scene = {};

        polyCount = 0;

        // Get objects
        for (auto &pair: entScene.getPool<MeshRenderComponent>()) {
            // TODO: Culling

            auto &transform = entScene.getComponent<TransformComponent>(pair.first);
            if (!transform.enabled)
                continue;

            auto &render = pair.second;
            if (!render.enabled)
                continue;

            polyCount += render.mesh.get().polyCount();

            Scene::Object node;
            node.transform = TransformComponent::walkHierarchy(transform, entScene);
            node.mesh = render.mesh;
            node.material = render.material;

            //TODO: Change transform walking / scene creation to allow model matrix caching
            scene.objects.emplace_back(node);
        }

        // Get skybox texture
        for (auto &pair: entScene.getPool<SkyboxComponent>()) {
            auto &comp = pair.second;
            scene.skybox.texture = comp.skybox.texture;
        }

        // Get Camera
        for (auto &pair: entScene.getPool<CameraComponent>()) {
            auto &tcomp = entScene.getComponent<TransformComponent>(pair.first);

            if (!tcomp.enabled)
                continue;

            auto &comp = pair.second;

            scene.camera = comp.camera;
            scene.cameraTransform = TransformComponent::walkHierarchy(tcomp, entScene);

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

            switch (lightComponent.type) {
                case LightComponent::LIGHT_DIRECTIONAL: {
                    auto tmp = std::get<DirectionalLight>(lightComponent.light);
                    tmp.transform = tcomp.transform;
                    scene.directionalLights.emplace_back(tmp);
                    break;
                }
                case LightComponent::LIGHT_POINT: {
                    auto tmp = std::get<PointLight>(lightComponent.light);
                    tmp.transform = tcomp.transform;
                    scene.pointLights.emplace_back(tmp);
                    break;
                }
                case LightComponent::LIGHT_SPOT: {
                    auto tmp = std::get<SpotLight>(lightComponent.light);
                    tmp.transform = tcomp.transform;
                    scene.spotLights.emplace_back(tmp);
                    break;
                }
            }
        }

        // Render
        pipeline.render(scene);
    }

    SceneRenderer &MeshRenderSystem::getPipeline() {
        return pipeline;
    }
}