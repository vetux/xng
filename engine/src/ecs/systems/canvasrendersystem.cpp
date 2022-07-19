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
    }

    void CanvasRenderSystem::stop(EntityScene &scene) {
        scene.removeListener(*this);
    }

    void CanvasRenderSystem::update(DeltaTime deltaTime, EntityScene &scene) {
        //TODO: Implement rect transform nesting support.

        std::map<int, std::vector<EntityHandle>> drawCalls;
        for (auto &p: scene.getComponentContainer().getPool<RectTransform>()) {
            if (!p.second.enabled)
                continue;
            auto &c = scene.lookupComponent<CanvasComponent>(scene.getByName(p.second.parent));
            drawCalls[c.layer].emplace_back(p.first);
        }

        for (auto &pair: drawCalls) {
            ren2d.renderBegin(target, false);
            for (auto &ent: pair.second) {
                auto &rt = scene.lookupComponent<RectTransform>(ent);
                auto &t = scene.lookupComponent<CanvasComponent>(scene.getByName(rt.parent));
                ren2d.setCameraPosition(t.cameraPosition);

                if (scene.checkComponent<SpriteComponent>(ent)) {
                    auto &comp = scene.lookupComponent<SpriteComponent>(ent);
                    if (comp.sprite) {
                        auto dstRect = Rectf(rt.rect.position +
                                             RectTransform::getOffset(rt.anchor,
                                                                      target.getDescription().size.convert<float>()),
                                             rt.rect.dimensions);
                        ren2d.draw(dstRect,
                                   *spriteTextures.at(ent),
                                   comp.offset,
                                   rt.rotation);
                    }
                } else if (scene.checkComponent<TextComponent>(ent)) {
                    auto &comp = scene.lookupComponent<TextComponent>(ent);
                    if (!comp.text.empty()) {
                        auto texSize = renderedTexts.at(ent).getTexture().getDescription().size.convert<float>();
                        Vec2f displaySize(0);
                        Vec2f displayOffset = RectTransform::getOffset(rt.anchor,
                                                                       target.getDescription().size.convert<float>());
                        if (texSize.x > rt.rect.dimensions.x) {
                            displaySize.x = rt.rect.dimensions.x;
                        } else {
                            displaySize.x = texSize.x;
                            displayOffset.x = (rt.rect.dimensions.x - texSize.x) / 2;
                        }
                        if (texSize.y > rt.rect.dimensions.y) {
                            displaySize.y = rt.rect.dimensions.y;
                        } else {
                            displaySize.y = texSize.y;
                            displayOffset.y = (rt.rect.dimensions.y - texSize.y) / 2;
                        }
                        auto dstRect = Rectf(rt.rect.position + displayOffset, displaySize);
                        auto center = Vec2f(displaySize.x / 2, displaySize.y / 2);
                        ren2d.draw(renderedTexts.at(ent), dstRect, ColorRGBA::fromVector(comp.textColor), center);
                    }
                }
            }
            ren2d.renderPresent();
        }
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
        if (t.sprite) {
            spriteTextures[ent] = ren2d.getDevice().createTextureBuffer({t.sprite.get().offset.dimensions});
            auto &img = t.sprite.get().image.get();
            if (img.getSize() != t.sprite.get().offset.dimensions) {
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
                textRenderers.insert(std::make_pair(comp.fontPath, TextRenderer(*fonts[comp.fontPath], ren2d)));
            }
            fonts.at(comp.fontPath)->setPixelSize(comp.pixelSize);
            renderedTexts.insert(std::make_pair(ent, textRenderers.at(comp.fontPath).render(comp.text, comp.lineHeight,
                                                                                            comp.lineWidth,
                                                                                            comp.lineSpacing)));
        }
    }
}