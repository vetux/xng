/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_SPRITERENDERSYSTEM_HPP
#define XENGINE_SPRITERENDERSYSTEM_HPP

#include <map>
#include <string>

#include "ecs/system.hpp"
#include "ecs/components/spriterendercomponent.hpp"

#include "render/2d/renderer2d.hpp"

namespace xng {
    class XENGINE_EXPORT SpriteRenderSystem : public System, public EntityScene::Listener {
    public:
        explicit SpriteRenderSystem(Renderer2D &renderer2D, RenderTarget &target);

        ~SpriteRenderSystem() override = default;

        void start(EntityScene &scene) override;

        void stop(EntityScene &scene) override;

        void update(DeltaTime deltaTime, EntityScene &scene) override;

        void onComponentCreate(const EntityHandle &entity,
                               const std::any &component,
                               std::type_index componentType) override;

        void onComponentDestroy(const EntityHandle &entity,
                                const std::any &component,
                                std::type_index componentType) override;

        void onComponentUpdate(const EntityHandle &entity,
                               const std::any &oldComponent,
                               const std::any &newComponent,
                               std::type_index componentType) override;

    private:
        void createTexture(const EntityHandle &ent, const SpriteRenderComponent &comp);

        Mesh getPlane(const Rectf &displayRect);

        Renderer2D &ren2d;
        RenderTarget &target;

        std::map<EntityHandle, std::unique_ptr<TextureBuffer>> textures;
    };
}

#endif //XENGINE_SPRITERENDERSYSTEM_HPP
