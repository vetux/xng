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

#ifndef XENGINE_RENDERSYSTEM_HPP
#define XENGINE_RENDERSYSTEM_HPP

#include <map>
#include <string>

#include "ecs/system.hpp"
#include "ecs/components/meshrendercomponent.hpp"
#include "ecs/components/skyboxcomponent.hpp"

#include "render/pipeline.hpp"

namespace xengine {
    class XENGINE_EXPORT RenderSystem : public System,
                                        ComponentPool<MeshRenderComponent>::Listener,
                                        ComponentPool<SkyboxComponent>::Listener {
    public:
        RenderSystem(RenderTarget &screen,
                     Pipeline &pipeline);

        ~RenderSystem() override;

        void start(EntityManager &entityManager) override;

        void stop(EntityManager &entityManager) override;

        void update(float deltaTime, EntityManager &entityManager) override;

        Pipeline &getPipeline();

        Scene &getScene();

        size_t getPolyCount() const { return polyCount; }

    private:
        void onComponentCreate(const Entity &entity, const MeshRenderComponent &component) override;

        void onComponentDestroy(const Entity &entity, const MeshRenderComponent &component) override;

        void onComponentCreate(const Entity &entity, const SkyboxComponent &component) override;

        void onComponentDestroy(const Entity &entity, const SkyboxComponent &component) override;

        void onComponentUpdate(const Entity &entity,
                               const MeshRenderComponent &oldValue,
                               const MeshRenderComponent &newValue) override;

        void onComponentUpdate(const Entity &entity,
                               const SkyboxComponent &oldValue,
                               const SkyboxComponent &newValue) override;

        Pipeline &pipeline;
        RenderTarget &screenTarget;

        Scene scene;

        size_t polyCount{};
    };
}

#endif //XENGINE_RENDERSYSTEM_HPP
