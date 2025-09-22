/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/ecs/systems/rendersystem.hpp"
#include "xng/ecs/components.hpp"
#include "xng/graphics/renderscene.hpp"
#include "xng/graphics/3d/passes/canvasrenderpass.hpp"
#include "xng/graphics/3d/passes/compositingpass.hpp"
#include "xng/graphics/3d/passes/constructionpass.hpp"
#include "xng/util/time.hpp"

namespace xng {
    RenderSystem::RenderSystem(std::shared_ptr<RenderGraphRuntime> renderGraphRuntime)
        : runtime(std::move(renderGraphRuntime)),
          scheduler(runtime),
          registry(std::make_shared<SharedResourceRegistry>()),
          config(std::make_shared<RenderConfiguration>()) {
        graph = scheduler.addGraph({
            std::make_shared<ConstructionPass>(config, registry),
            std::make_shared<CanvasRenderPass>(config, registry),
            std::make_shared<CompositingPass>(config, registry),
        });
    }

    RenderSystem::~RenderSystem() = default;

    void RenderSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        // Build Canvases
        std::vector<Canvas> canvases;

        config->setCanvases(canvases);

        RenderScene renderScene = {};

        // Add canvas textures to the scene

        // TODO: Culling
        // TODO: Z Sort transparent meshes based on distance of transform to camera
        // TODO: Change transform walking / scene creation to allow model matrix caching

        // Get Meshes
        for (auto &pair: scene.getPool<MeshComponent>()) {
            auto &transform = scene.getComponent<TransformComponent>(pair.entity);
            if (!transform.enabled)
                continue;

            if (!pair.component.enabled)
                continue;

            MeshObject object;
            object.transform = TransformComponent::getAbsoluteTransform(transform, scene);
            object.mesh = pair.component.mesh;
            object.castShadows = pair.component.castShadows;
            object.receiveShadows = pair.component.receiveShadows;

            if (scene.checkComponent<MaterialComponent>(pair.entity)) {
                object.materials = scene.getComponent<MaterialComponent>(pair.entity).materials;
            }

            renderScene.meshes.push_back(std::move(object));
        }

        // Get Rigged Meshes
        for (auto &pair: scene.getPool<SkinnedMeshComponent>()) {
            auto &transform = scene.getComponent<TransformComponent>(pair.entity);
            if (!transform.enabled)
                continue;

            if (!pair.component.enabled)
                continue;

            SkinnedMeshObject object;
            object.transform = TransformComponent::getAbsoluteTransform(transform, scene);
            object.mesh = pair.component.mesh;
            object.castShadows = pair.component.castShadows;
            object.receiveShadows = pair.component.receiveShadows;

            if (scene.checkComponent<MaterialComponent>(pair.entity)) {
                object.materials = scene.getComponent<MaterialComponent>(pair.entity).materials;
            }

            if (scene.checkComponent<RigAnimationComponent>(pair.entity)) {
                object.boneTransforms = scene.getComponent<RigAnimationComponent>(pair.entity).boneTransforms;
            }

            renderScene.skinnedMeshes.push_back(std::move(object));
        }

        // Get skybox
        for (auto &pair: scene.getPool<SkyboxComponent>()) {
            renderScene.skybox = pair.component.skybox;
        }

        // Get Camera
        for (auto &pair: scene.getPool<CameraComponent>()) {
            auto &transform = scene.getComponent<TransformComponent>(pair.entity);

            if (!transform.enabled)
                continue;

            renderScene.camera = pair.component.camera;
            renderScene.cameraTransform = TransformComponent::getAbsoluteTransform(transform, scene);

            break;
        }

        // Get point lights
        for (auto &pair: scene.getPool<PointLightComponent>()) {
            auto &transform = scene.getPool<TransformComponent>().lookup(pair.entity);

            if (!pair.component.enabled)
                continue;

            if (!transform.enabled)
                continue;

            PointLightObject object;
            object.transform = transform.transform;
            object.light = pair.component.light;
            object.castShadows = pair.component.castShadows;

            renderScene.pointLights.emplace_back(std::move(object));
        }

        // Get spotlights
        for (auto &pair: scene.getPool<SpotLightComponent>()) {
            auto &transform = scene.getPool<TransformComponent>().lookup(pair.entity);

            if (!pair.component.enabled)
                continue;

            if (!transform.enabled)
                continue;

            SpotLightObject object;
            object.transform = transform.transform;
            object.light = pair.component.light;
            object.castShadows = pair.component.castShadows;

            renderScene.spotLights.emplace_back(std::move(object));
        }

        // Get directional lights
        for (auto &pair: scene.getPool<DirectionalLightComponent>()) {
            auto &transform = scene.getPool<TransformComponent>().lookup(pair.entity);

            if (!pair.component.enabled)
                continue;

            if (!transform.enabled)
                continue;

            DirectionalLightObject object;
            object.transform = transform.transform;
            object.light = pair.component.light;
            object.castShadows = pair.component.castShadows;

            renderScene.directionalLights.emplace_back(std::move(object));
        }

        // Get Sprites
        for (auto &pair: scene.getPool<SpriteComponent>()) {
            auto &transform = scene.getPool<TransformComponent>().lookup(pair.entity);

            if (!pair.component.enabled)
                continue;

            if (!transform.enabled)
                continue;

            SpriteObject object;
            object.transform = transform.transform;
            object.sprite = pair.component.sprite;
            object.textureFiltering = pair.component.filter;

            renderScene.sprites.emplace_back(std::move(object));
        }

        // Get Animated Sprites
        for (auto &pair: scene.getPool<SpriteAnimationComponent>()) {
            auto &transform = scene.getPool<TransformComponent>().lookup(pair.entity);

            if (!pair.component.enabled)
                continue;

            if (!transform.enabled)
                continue;

            SpriteObject object;
            object.transform = transform.transform;
            object.sprite = pair.component.sprite;
            object.textureFiltering = pair.component.filter;

            renderScene.sprites.emplace_back(std::move(object));
        }

        config->setScene(renderScene);

        // Execute the graph
        scheduler.execute(graph);
    }
}
