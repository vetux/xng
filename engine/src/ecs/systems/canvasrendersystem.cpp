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

#include "ecs/systems/canvasrendersystem.hpp"

#include "ecs/components/transformcomponent.hpp"
#include "ecs/components/recttransform.hpp"
#include "ecs/components/canvascomponent.hpp"
#include "ecs/components/textcomponent.hpp"

namespace xng {
    CanvasRenderSystem::CanvasRenderSystem(Renderer2D &renderer2D,
                                           RenderTarget &target,
                                           FontDriver &fontDriver,
                                           Archive &archive)
            : ren2d(renderer2D), target(target), fontDriver(fontDriver), archive(archive) {}

    void CanvasRenderSystem::start(EntityScene &scene) {
        scene.addListener(*this);
        for (auto &pair: scene.getPool<SpriteComponent>()) {
            if (!pair.second.sprite.assigned())
                continue;
            if (spriteTextures.find(pair.first) == spriteTextures.end()) {
                createTexture(pair.first, pair.second);
            }
        }
        for (auto &pair: scene.getPool<TextComponent>()) {
            if (renderedTexts.find(pair.first) == renderedTexts.end()) {
                createText(pair.first, pair.second);
            }
        }
    }

    void CanvasRenderSystem::stop(EntityScene &scene) {
        scene.removeListener(*this);
        spriteTextures.clear();
        textRenderers.clear();
        renderedTexts.clear();
        fonts.clear();
    }

    void CanvasRenderSystem::update(DeltaTime deltaTime, EntityScene &scene) {
        //TODO: Implement rect transform nesting support.

        std::map<int, std::map<int, std::vector<EntityHandle>>> canvases;
        for (auto &p: scene.getPool<RectTransform>()) {
            if (!p.second.enabled)
                continue;
            auto &c = scene.lookup<CanvasComponent>(scene.getEntityByName(p.second.parent));
            if (scene.check<SpriteComponent>(p.first)) {
                auto &r = scene.lookup<SpriteComponent>(p.first);
                canvases[c.layer][r.layer].emplace_back(p.first);
            } else if (scene.check<TextComponent>(p.first)) {
                auto &r = scene.lookup<TextComponent>(p.first);
                canvases[c.layer][r.layer].emplace_back(p.first);
            } else {
                canvases[c.layer][0].emplace_back(p.first);
            }
        }

        ren2d.renderBegin(target, false);

        for (auto &pair: canvases) {
            for (auto &entPair: pair.second) {
                for (auto &ent: entPair.second) {
                    auto &rt = scene.lookup<RectTransform>(ent);
                    auto &t = scene.lookup<CanvasComponent>(scene.getEntityByName(rt.parent));
                    ren2d.setCameraPosition(t.cameraPosition);

                    if (scene.check<SpriteComponent>(ent)) {
                        auto &comp = scene.lookup<SpriteComponent>(ent);
                        if (comp.sprite.assigned()) {
                            auto dstRect = Rectf(rt.rect.position +
                                                 RectTransform::getOffset(rt.anchor,
                                                                          target.getDescription().size.convert<float>()),
                                                 rt.rect.dimensions);
                            ren2d.draw(Rectf({}, dstRect.dimensions),
                                       dstRect,
                                       *spriteTextures.at(ent),
                                       rt.center,
                                       rt.rotation,
                                       comp.flipSprite);
                        }
                    } else if (scene.check<TextComponent>(ent)) {
                        auto &comp = scene.lookup<TextComponent>(ent);
                        if (!comp.text.empty()) {
                            auto texSize = renderedTexts.at(ent).getTexture().getDescription().size.convert<float>();
                            Vec2f displaySize(0);
                            Vec2f displayOffset = RectTransform::getOffset(rt.anchor,
                                                                           target.getDescription().size.convert<float>());
                            if (texSize.x > rt.rect.dimensions.x) {
                                displaySize.x = rt.rect.dimensions.x;
                            } else {
                                displaySize.x = texSize.x;
                                displayOffset.x += (rt.rect.dimensions.x - texSize.x) / 2;
                            }
                            if (texSize.y > rt.rect.dimensions.y) {
                                displaySize.y = rt.rect.dimensions.y;
                            } else {
                                displaySize.y = texSize.y;
                                displayOffset.y -= (rt.rect.dimensions.y - texSize.y) / 2;
                            }
                            auto dstRect = Rectf(rt.rect.position + displayOffset, displaySize);
                            auto center = Vec2f(displaySize.x / 2, displaySize.y / 2);
                            ren2d.draw(renderedTexts.at(ent), dstRect, comp.textColor, center, rt.rotation);
                        }
                    }
                }
            }
        }
        ren2d.renderPresent();
    }

    void CanvasRenderSystem::onComponentCreate(const EntityHandle &entity,
                                               const std::any &component,
                                               std::type_index componentType) {
        if (componentType == typeid(SpriteComponent)) {
            auto *t = std::any_cast<SpriteComponent>(&component);
            createTexture(entity, *t);
        } else if (componentType == typeid(TextComponent)) {
            auto *t = std::any_cast<TextComponent>(&component);
            createText(entity, *t);
        }
    }

    void CanvasRenderSystem::onComponentDestroy(const EntityHandle &entity,
                                                const std::any &component,
                                                std::type_index componentType) {
        if (componentType == typeid(SpriteComponent)) {
            spriteTextures.erase(entity);
        } else if (componentType == typeid(TextComponent)) {
            renderedTexts.erase(entity);
        }
    }

    void CanvasRenderSystem::onComponentUpdate(const EntityHandle &entity,
                                               const std::any &oldComponent,
                                               const std::any &newComponent,
                                               std::type_index componentType) {
        if (componentType == typeid(SpriteComponent)) {
            auto *os = std::any_cast<SpriteComponent>(&oldComponent);
            auto *ns = std::any_cast<SpriteComponent>(&newComponent);
            if (os->sprite != ns->sprite) {
                spriteTextures.erase(entity);
                createTexture(entity, *ns);
            }
        } else if (componentType == typeid(TextComponent)) {
            auto *ns = std::any_cast<TextComponent>(&newComponent);
            renderedTexts.erase(entity);
            createText(entity, *ns);
        }
    }

    void CanvasRenderSystem::createTexture(const EntityHandle &ent, const SpriteComponent &t) {
        if (t.sprite.assigned()) {
            Vec2i dimensions = t.sprite.get().offset.dimensions;
            if (dimensions.x * dimensions.y == 0) {
                dimensions = t.sprite.get().image.get().getSize();
            }
            spriteTextures[ent] = ren2d.getDevice().createTextureBuffer({dimensions});
            auto &img = t.sprite.get().image.get();
            if (img.getSize() != dimensions) {
                // Upload a slice of an image
                auto slice = img.slice(t.sprite.get().offset);
                spriteTextures[ent]->upload(slice);
            } else {
                // Upload the whole image
                spriteTextures[ent]->upload(img);
            }
        }
    }

    void CanvasRenderSystem::createText(const EntityHandle &ent, const TextComponent &comp) {
        if (!comp.text.empty()) {
            auto it = fonts.find(comp.fontPath);
            if (it == fonts.end()) {
                auto stream = archive.open(comp.fontPath);
                fonts[comp.fontPath] = fontDriver.createFont(*stream);
            }
            auto rIt = textRenderers.find(ent);
            if (rIt == textRenderers.end()) {
                fonts.at(comp.fontPath)->setPixelSize(comp.pixelSize);
                textRenderers.insert(std::make_pair(ent, TextRenderer(*fonts[comp.fontPath], ren2d)));
            }
            renderedTexts.insert(std::make_pair(ent, textRenderers.at(ent).render(comp.text, comp.lineHeight,
                                                                                  comp.lineWidth,
                                                                                  comp.lineSpacing)));
        }
    }
}