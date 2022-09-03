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

#ifndef XENGINE_CANVASRENDERSYSTEM_HPP
#define XENGINE_CANVASRENDERSYSTEM_HPP

#include "ecs/system.hpp"
#include "ecs/components/spritecomponent.hpp"
#include "ecs/components/textcomponent.hpp"

#include "text/fontdriver.hpp"
#include "text/textrenderer.hpp"

#include "render/2d/renderer2d.hpp"

namespace xng {
    /**
     * The canvas render system handles drawing of 2d elements
     */
    class XENGINE_EXPORT CanvasRenderSystem : public System, public EntityScene::Listener {
    public:
        CanvasRenderSystem(Renderer2D &renderer2D,
                           RenderTarget &target,
                           FontDriver &fontDriver);

        ~CanvasRenderSystem() override = default;

        void start(EntityScene &scene) override;

        void stop(EntityScene &scene) override;

        void update(DeltaTime deltaTime, EntityScene &scene) override;

        void setDrawDebug(bool v) { drawDebug = v; }

        void onComponentCreate(const EntityHandle &entity, const std::any &component) override;

        void onComponentDestroy(const EntityHandle &entity, const std::any &component) override;

        void onComponentUpdate(const EntityHandle &entity, const std::any &oldComponent,
                               const std::any &newComponent) override;

    private:
        void createTexture(const EntityHandle &ent, const SpriteComponent &comp);

        void createText(const EntityHandle &ent, const TextComponent &comp);

        Renderer2D &ren2d;
        RenderTarget &target;
        FontDriver &fontDriver;

        bool drawDebug = false;

        std::map<Uri, std::unique_ptr<Font>> fonts;
        std::map<EntityHandle, TextRenderer> textRenderers;
        std::map<EntityHandle, std::unique_ptr<TextureBuffer>> spriteTextures;
        std::map<EntityHandle, std::unique_ptr<TextureBuffer>> spriteTexturesB;
        std::map<EntityHandle, Text> renderedTexts;
    };
}
#endif //XENGINE_CANVASRENDERSYSTEM_HPP
