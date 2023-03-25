/**
 *  This file is part of xEngine, a C++ game engine library.
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

#ifndef XENGINE_CANVASRENDERSYSTEM_HPP
#define XENGINE_CANVASRENDERSYSTEM_HPP

#include "xng/ecs/system.hpp"
#include "xng/ecs/components/spritecomponent.hpp"
#include "xng/ecs/components/textcomponent.hpp"

#include "xng/font/fontdriver.hpp"

#include "xng/gui/textrenderer.hpp"
#include "xng/gui/canvasscalingmode.hpp"

#include "xng/render/2d/renderer2d.hpp"

namespace xng {
    class XENGINE_EXPORT CanvasRenderSystem : public System, public EntityScene::Listener {
    public:
        CanvasRenderSystem(Renderer2D &renderer2D,
                           RenderTarget &target,
                           FontDriver &fontDriver,
                           bool drawDebugGeometry = false);

        ~CanvasRenderSystem() override = default;

        CanvasRenderSystem(const CanvasRenderSystem &other) = delete;

        CanvasRenderSystem &operator=(const CanvasRenderSystem &other) = delete;

        void start(EntityScene &scene, EventBus &eventBus) override;

        void stop(EntityScene &scene, EventBus &eventBus) override;

        void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) override;

        void onComponentCreate(const EntityHandle &entity, const Component &component) override;

        void onComponentDestroy(const EntityHandle &entity, const Component &component) override;

        void onComponentUpdate(const EntityHandle &entity,
                               const Component &oldComponent,
                               const Component &newComponent) override;

        std::string getName() override { return "CanvasRenderSystem"; }

        void setDrawDebugGeometry(bool v) { drawDebugGeometry = v; }

    private:
        void createTexture(const EntityHandle &ent, const SpriteComponent &comp);

        void updateText(const EntityHandle &ent, const TextComponent &comp, const Vec2f &sizeScale);

        void destroyTextRenderer(const Vec2i &size);

        Renderer2D &ren2d;
        RenderTarget &target;
        FontDriver &fontDriver;

        bool drawDebugGeometry = false;

        std::map<EntityHandle, TextureAtlasHandle> spriteTextureHandles;

        std::map<Uri, std::unique_ptr<Font>> fonts;

        std::map<EntityHandle, Vec2i> textPixelSizes;
        std::unordered_map<Vec2i, TextRenderer> textRenderers;

        std::map<EntityHandle, Text> renderedTexts;
        std::map<EntityHandle, TextureAtlasHandle> textTextureHandles;
    };
}

#endif //XENGINE_CANVASRENDERSYSTEM_HPP
