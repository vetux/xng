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

#include <algorithm>
#include <filesystem>

#include "ecs/systems/meshrendersystem.hpp"
#include "ecs/components.hpp"

namespace xng {
    MeshRenderSystem::MeshRenderSystem(RenderTarget &screen,
                                       SceneRenderer &pipeline)
            : screenTarget(screen),
              pipeline(pipeline) {
    }

    MeshRenderSystem::~MeshRenderSystem() = default;

    void MeshRenderSystem::start(EntityScene &entityManager) {}

    void MeshRenderSystem::stop(EntityScene &entityManager) {}

    void MeshRenderSystem::update(DeltaTime deltaTime, EntityScene &entScene) {
        scene = {};

        polyCount = 0;

        // TODO: Culling

        // Create draw nodes
        for (auto &pair: entScene.getPool<MeshRenderComponent>()) {
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


        // Update skybox texture
        for (auto &pair: entScene.getPool<SkyboxComponent>()) {
            auto &comp = pair.second;
            scene.skybox.texture = comp.skybox.texture;
        }

        // Update Camera
        for (auto &pair: entScene.getPool<CameraComponent>()) {
            auto &tcomp = entScene.getComponent<TransformComponent>(pair.first);

            if (!tcomp.enabled)
                continue;

            auto comp = pair.second;

            comp.camera.aspectRatio = (float) screenTarget.getDescription().size.x
                                      / (float) screenTarget.getDescription().size.y;

            entScene.updateComponent(pair.first, comp);

            scene.camera = comp.camera;
            scene.cameraTransform = TransformComponent::walkHierarchy(tcomp, entScene);

            break;
        }

        // Update lights
        for (auto &pair: entScene.getPool<LightComponent>()) {
            auto lightComponent = pair.second;
            auto &tcomp = entScene.getPool<TransformComponent>().lookup(pair.first);

            if (!lightComponent.enabled)
                continue;

            if (!tcomp.enabled)
                continue;

            lightComponent.light.transform = tcomp.transform;

            entScene.updateComponent(pair.first, lightComponent);

            scene.lights.emplace_back(lightComponent.light);
        }

        // Render
        pipeline.render(screenTarget, scene);
    }

    SceneRenderer &MeshRenderSystem::getPipeline() {
        return pipeline;
    }

    Scene &MeshRenderSystem::getScene() {
        return scene;
    }
}