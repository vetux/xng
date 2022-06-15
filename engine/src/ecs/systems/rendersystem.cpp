/**
 *  xEngine - C++ game engine library
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
#include <filesystem>

#include "ecs/systems/rendersystem.hpp"
#include "ecs/components.hpp"

namespace xengine {
    RenderSystem::RenderSystem(RenderTarget &screen,
                               SceneRenderer &pipeline)
            : screenTarget(screen),
              pipeline(pipeline) {
    }

    RenderSystem::~RenderSystem() = default;

    void RenderSystem::start(EntityManager &entityManager) {
        entityManager.getComponentManager().getPool<MeshRenderComponent>().addListener(this);
        entityManager.getComponentManager().getPool<SkyboxComponent>().addListener(this);
    }

    void RenderSystem::stop(EntityManager &entityManager) {
        entityManager.getComponentManager().getPool<MeshRenderComponent>().removeListener(this);
        entityManager.getComponentManager().getPool<SkyboxComponent>().removeListener(this);
    }

    void RenderSystem::update(float deltaTime, EntityManager &entityManager) {
        auto &componentManager = entityManager.getComponentManager();

        scene = {};

        polyCount = 0;

        // TODO: Culling

        // Create draw nodes
        for (auto &pair: componentManager.getPool<MeshRenderComponent>()) {
            auto &transform = componentManager.lookup<TransformComponent>(pair.first);
            if (!transform.enabled)
                continue;

            auto &render = pair.second;
            if (!render.enabled)
                continue;

            polyCount += render.mesh.get().polyCount();

            Scene::Object node(render.mesh, render.material);
            node.transform = TransformComponent::walkHierarchy(transform, entityManager);

            //TODO: Change transform walking / scene creation to allow model matrix caching
            scene.objects.emplace_back(node);
        }

        // Update skybox texture
        for (auto &pair: componentManager.getPool<SkyboxComponent>()) {
            auto &comp = pair.second;
            scene.skybox.texture = comp.skybox.texture;
        }

        // Update Camera
        for (auto &pair: componentManager.getPool<CameraComponent>()) {
            auto &tcomp = componentManager.lookup<TransformComponent>(pair.first);

            if (!tcomp.enabled)
                continue;

            auto &comp = pair.second;

            comp.camera.aspectRatio =
                    (float) screenTarget.getDescription().size.x / (float) screenTarget.getDescription().size.y;

            scene.camera = comp.camera;
            scene.camera.transform = TransformComponent::walkHierarchy(tcomp, entityManager);

            break;
        }

        // Update lights
        for (auto &pair: componentManager.getPool<LightComponent>()) {
            auto &lightComponent = pair.second;
            auto &tcomp = componentManager.getPool<TransformComponent>().lookup(pair.first);

            if (!lightComponent.enabled)
                continue;

            if (!tcomp.enabled)
                continue;

            lightComponent.light.transform = tcomp.transform;

            scene.lights.emplace_back(lightComponent.light);
        }

        // Render
        pipeline.render(screenTarget, scene);
    }

    SceneRenderer &RenderSystem::getPipeline() {
        return pipeline;
    }

    Scene &RenderSystem::getScene() {
        return scene;
    }

    void RenderSystem::onComponentCreate(const Entity &entity, const MeshRenderComponent &component) {}

    void RenderSystem::onComponentDestroy(const Entity &entity, const MeshRenderComponent &component) {}

    void RenderSystem::onComponentCreate(const Entity &entity, const SkyboxComponent &component) {}

    void RenderSystem::onComponentDestroy(const Entity &entity, const SkyboxComponent &component) {}

    void RenderSystem::onComponentUpdate(const Entity &entity,
                                         const MeshRenderComponent &oldValue,
                                         const MeshRenderComponent &newValue) {}

    void RenderSystem::onComponentUpdate(const Entity &entity,
                                         const SkyboxComponent &oldValue,
                                         const SkyboxComponent &newValue) {}
}