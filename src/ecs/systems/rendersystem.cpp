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
                               RenderDevice &device,
                               Archive &archive,
                               AssetManager &assetManager,
                               AssetRenderManager &assetRenderManager,
                               Pipeline &pipeline)
            : screenTarget(screen),
              device(device),
              archive(archive),
              assetManager(assetManager),
              assetRenderManager(assetRenderManager),
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

        Scene scene;

        polyCount = 0;

        //TODO: Culling
        //Create deferred draw nodes
        for (auto &pair: componentManager.getPool<MeshRenderComponent>()) {
            auto &transform = componentManager.lookup<TransformComponent>(pair.first);
            if (!transform.enabled)
                continue;

            auto &render = pair.second;
            if (!render.enabled)
                continue;

            auto mesh = AssetHandle<Mesh>(render.mesh, assetManager, &assetRenderManager);
            auto material = AssetHandle<Material>(render.material, assetManager);

            polyCount += mesh.get().polyCount();

            //TODO: Change transform walking / scene creation to allow model matrix caching
            scene.deferred.emplace_back(Scene::DeferredDrawNode(
                    TransformComponent::walkHierarchy(transform, entityManager),
                    mesh,
                    material));
        }

        //Get Skybox
        for (auto &pair: componentManager.getPool<SkyboxComponent>()) {
            auto &comp = pair.second;
            scene.skybox = comp.skybox;
        }

        //Get Camera
        for (auto &pair: componentManager.getPool<CameraComponent>()) {
            auto &tcomp = componentManager.lookup<TransformComponent>(pair.first);

            if (!tcomp.enabled)
                continue;

            auto &comp = pair.second;

            scene.camera = comp.camera;
            scene.camera.transform = TransformComponent::walkHierarchy(tcomp, entityManager);

            break;
        }

        //Get lights
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

        //Render
        pipeline.render(screenTarget, scene);
    }

    Pipeline &RenderSystem::getPipeline() {
        return pipeline;
    }

    void RenderSystem::onComponentCreate(const Entity &entity, const MeshRenderComponent &component) {
        assetManager.incrementRef(component.mesh);
        assetManager.incrementRef(component.material);

        auto material = assetManager.getAsset<Material>(component.material);

        assetRenderManager.incrementRef(component.mesh);

        if (!material.diffuseTexture.empty()) {
            assetRenderManager.incrementRef(material.diffuseTexture);
        }
        if (!material.ambientTexture.empty()) {
            assetRenderManager.incrementRef(material.ambientTexture);
        }
        if (!material.specularTexture.empty()) {
            assetRenderManager.incrementRef(material.specularTexture);
        }
        if (!material.emissiveTexture.empty()) {
            assetRenderManager.incrementRef(material.emissiveTexture);
        }
        if (!material.shininessTexture.empty()) {
            assetRenderManager.incrementRef(material.shininessTexture);
        }
        if (!material.normalTexture.empty()) {
            assetRenderManager.incrementRef(material.normalTexture);
        }
    }

    void RenderSystem::onComponentDestroy(const Entity &entity, const MeshRenderComponent &component) {
        assetRenderManager.decrementRef<Mesh>(component.mesh);
        auto material = assetManager.getAsset<Material>(component.material);
        if (!material.diffuseTexture.empty()) {
            assetRenderManager.decrementRef<Texture>(material.diffuseTexture);
        }
        if (!material.ambientTexture.empty()) {
            assetRenderManager.decrementRef<Texture>(material.ambientTexture);
        }
        if (!material.specularTexture.empty()) {
            assetRenderManager.decrementRef<Texture>(material.specularTexture);
        }
        if (!material.emissiveTexture.empty()) {
            assetRenderManager.decrementRef<Texture>(material.emissiveTexture);
        }
        if (!material.shininessTexture.empty()) {
            assetRenderManager.decrementRef<Texture>(material.shininessTexture);
        }
        if (!material.normalTexture.empty()) {
            assetRenderManager.decrementRef<Texture>(material.normalTexture);
        }
        assetManager.decrementRef(component.material);
        assetManager.decrementRef(component.mesh);
    }

    void RenderSystem::onComponentCreate(const Entity &entity, const SkyboxComponent &component) {
        assetRenderManager.incrementRef(component.skybox.texture);
    }

    void RenderSystem::onComponentDestroy(const Entity &entity, const SkyboxComponent &component) {
        assetRenderManager.decrementRef<TextureBuffer>(component.skybox.texture);
    }

    void RenderSystem::onComponentUpdate(const Entity &entity,
                                         const MeshRenderComponent &oldValue,
                                         const MeshRenderComponent &newValue) {
        if (oldValue == newValue)
            return;
        onComponentDestroy(entity, oldValue);
        onComponentCreate(entity, newValue);
    }

    void RenderSystem::onComponentUpdate(const Entity &entity,
                                         const SkyboxComponent &oldValue,
                                         const SkyboxComponent &newValue) {
        if (oldValue == newValue)
            return;
        onComponentDestroy(entity, oldValue);
        onComponentCreate(entity, newValue);
    }
}