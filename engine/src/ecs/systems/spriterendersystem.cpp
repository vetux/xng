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

#include "ecs/systems/spriterendersystem.hpp"

#include "ecs/components/spritecameracomponent.hpp"
#include "ecs/components/transformcomponent.hpp"

namespace xng {
    SpriteRenderSystem::SpriteRenderSystem(Renderer2D &renderer2D, RenderTarget &target)
            : ren2d(renderer2D), target(target) {}

    void SpriteRenderSystem::start(EntityScene &scene) {
        scene.addListener(*this);
    }

    void SpriteRenderSystem::stop(EntityScene &scene) {
        scene.removeListener(*this);
    }

    void SpriteRenderSystem::update(DeltaTime deltaTime, EntityScene &scene) {
        std::map<int, std::vector<EntityHandle>> sprites;
        for (auto &p: scene.getComponentContainer().getPool<SpriteRenderComponent>()) {
            if (!p.second.enabled)
                continue;
            sprites[p.second.layer].emplace_back(p.first);
        }

        Vec2f cameraPosition = {};
        for (auto &p: scene.getPool<SpriteCameraComponent>()) {
            if (!p.second.enabled)
                continue;
            auto &t = scene.lookupComponent<TransformComponent>(p.first);
            cameraPosition = {t.transform.getPosition().x, t.transform.getPosition().y};
            break;
        }

        // Draw sprites
        ren2d.renderBegin(target);
        ren2d.setCameraPosition(cameraPosition);
        for (auto &layer: sprites) {
            for (auto &ent: layer.second) {
                auto &tcomp = scene.lookupComponent<TransformComponent>(ent);
                auto &comp = scene.lookupComponent<SpriteRenderComponent>(ent);
                auto &offset = comp.displayRect;
                Rectf dstRect(Vec2f(tcomp.transform.getPosition().x, tcomp.transform.getPosition().y),
                              comp.displayRect.dimensions);
                ren2d.draw(dstRect, *textures.at(ent), comp.displayRect.position,
                           tcomp.transform.getRotation().getEulerAngles().z);
            }
        }
        ren2d.renderPresent();
    }

    void SpriteRenderSystem::onComponentCreate(const EntityHandle &entity,
                                               const std::any &component,
                                               std::type_index componentType) {
        if (componentType == typeid(SpriteRenderComponent)) {
            auto *t = std::any_cast<SpriteRenderComponent>(&component);
            createTexture(entity, *t);
        }
    }

    void SpriteRenderSystem::onComponentDestroy(const EntityHandle &entity,
                                                const std::any &component,
                                                std::type_index componentType) {
        if (componentType == typeid(SpriteRenderComponent)) {
            textures.erase(entity);
        }
    }

    void SpriteRenderSystem::onComponentUpdate(const EntityHandle &entity,
                                               const std::any &oldComponent,
                                               const std::any &newComponent,
                                               std::type_index componentType) {
        if (componentType == typeid(SpriteRenderComponent)) {
            auto *os = std::any_cast<SpriteRenderComponent>(&oldComponent);
            auto *ns = std::any_cast<SpriteRenderComponent>(&newComponent);
            if (os->sprite != ns->sprite) {
                textures.erase(entity);
                createTexture(entity, *ns);
            }
        }
    }

    void SpriteRenderSystem::createTexture(const EntityHandle &ent, const SpriteRenderComponent &t) {
        if (t.sprite) {
            textures[ent] = ren2d.getDevice().createTextureBuffer({t.sprite.get().offset.dimensions});
            auto &img = t.sprite.get().image.get();
            if (img.getSize() != t.sprite.get().offset.dimensions) {
                // Upload a slice of an image
                auto slice = img.slice(t.sprite.get().offset);
                textures[ent]->upload(slice);
            } else {
                // Upload the whole image
                textures[ent]->upload(img);
            }
        }
    }
}