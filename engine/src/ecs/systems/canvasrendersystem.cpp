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
#include "ecs/components/recttransformcomponent.hpp"
#include "ecs/components/canvascomponent.hpp"
#include "ecs/components/textcomponent.hpp"

#include "ecs/components/rigidbodycomponent.hpp"

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

    struct Pass {
        enum Type {
            SPRITE,
            TEXT
        } type;
        EntityHandle ent;
    };

    void CanvasRenderSystem::update(DeltaTime deltaTime, EntityScene &scene) {
        //TODO: Implement rect transform nesting support.

        std::map<int, std::map<int, std::vector<Pass>>> passes;

        for (auto &p: scene.getPool<RectTransformComponent>()) {
            if (!p.second.enabled)
                continue;
            auto &c = scene.lookup<CanvasComponent>(scene.getEntityByName(p.second.parent));

            if (scene.check<SpriteComponent>(p.first)) {
                auto &r = scene.lookup<SpriteComponent>(p.first);
                passes[c.layer][r.layer].emplace_back(Pass{Pass::SPRITE, p.first});
            }

            if (scene.check<TextComponent>(p.first)) {
                auto &r = scene.lookup<TextComponent>(p.first);
                passes[c.layer][r.layer].emplace_back(Pass{Pass::TEXT, p.first});
            }
        }

        ren2d.renderBegin(target, false);

        for (auto &pair: passes) {
            for (auto &entPair: pair.second) {
                for (auto &pass: entPair.second) {
                    auto &rt = scene.lookup<RectTransformComponent>(pass.ent);
                    auto &t = scene.lookup<CanvasComponent>(scene.getEntityByName(rt.parent));

                    ren2d.setCameraPosition(t.cameraPosition);

                    Rectf dstRect;
                    Vec2f center;
                    float rotation = rt.rotation;

                    switch (pass.type) {
                        case Pass::SPRITE: {
                            auto &comp = scene.lookup<SpriteComponent>(pass.ent);
                            if (comp.sprite.assigned()) {
                                dstRect = Rectf(rt.rect.position +
                                                RectTransformComponent::getOffset(rt.anchor,
                                                                                  target.getDescription().size.convert<float>()),
                                                rt.rect.dimensions);
                                center = rt.center;
                                if (scene.check<TransformComponent>(pass.ent)) {
                                    auto &vec = scene.lookup<TransformComponent>(pass.ent);
                                    dstRect.position.x -= vec.transform.getPosition().x;
                                    dstRect.position.y -= vec.transform.getPosition().y;
                                    rotation += vec.transform.getRotation().getEulerAngles().z;
                                }
                                ren2d.draw(Rectf({}, dstRect.dimensions),
                                           dstRect,
                                           *spriteTextures.at(pass.ent),
                                           rt.center,
                                           rotation,
                                           comp.flipSprite);
                            }
                            break;
                        }
                        case Pass::TEXT: {
                            auto &tcomp = scene.lookup<TextComponent>(pass.ent);
                            if (!tcomp.text.empty()) {
                                auto texSize = renderedTexts.at(
                                        pass.ent).getTexture().getDescription().size.convert<float>();
                                Vec2f displaySize(0);
                                Vec2f displayOffset = RectTransformComponent::getOffset(rt.anchor,
                                                                                        target.getDescription().size.convert<float>());
                                if (texSize.x > rt.rect.dimensions.x) {
                                    displaySize.x = rt.rect.dimensions.x;
                                } else {
                                    displaySize.x = texSize.x;
                                }
                                if (texSize.y > rt.rect.dimensions.y) {
                                    displaySize.y = rt.rect.dimensions.y;
                                } else {
                                    displaySize.y = texSize.y;
                                }
                                dstRect = Rectf(rt.rect.position + displayOffset, displaySize);
                                center = Vec2f(displaySize.x / 2, displaySize.y / 2);
                                if (scene.check<TransformComponent>(pass.ent)) {
                                    auto &vec = scene.lookup<TransformComponent>(pass.ent);
                                    dstRect.position.x -= vec.transform.getPosition().x;
                                    dstRect.position.y -= vec.transform.getPosition().y;
                                    rotation += vec.transform.getRotation().z;
                                }
                                ren2d.draw(renderedTexts.at(pass.ent), dstRect, tcomp.textColor, center, rotation);
                            }
                            break;
                        }
                    }

                    if (drawDebug) {
                        ren2d.draw(dstRect, ColorRGBA::yellow(), false, center, rotation);
                        auto dCenter = dstRect.position;
                        const int len = 20;
                        ren2d.draw(dCenter + Vec2f(-len, 0), dCenter + Vec2f(len, 0), ColorRGBA::red());
                        ren2d.draw(dCenter + Vec2f(0, len), dCenter + Vec2f(0, -len), ColorRGBA::red());

                        if (scene.check<RigidBodyComponent>(pass.ent)) {
                            auto &rb = scene.lookup<RigidBodyComponent>(pass.ent);
                            if (rb.is2D) {
                                auto &tcomp = scene.lookup<TransformComponent>(pass.ent);
                                for (auto &col: rb.colliders) {
                                    if (!col.shape.vertices.empty()) {
                                        std::vector<Vec2f> poly;
                                        for (auto &vert: col.shape.vertices) {
                                            poly.emplace_back(Vec2f(vert.x, -vert.y));
                                        }
                                        ren2d.draw(poly,
                                                   Vec2f(-tcomp.transform.getPosition().x,
                                                         -tcomp.transform.getPosition().y),
                                                   col.isSensor ? ColorRGBA(0, 153, 255, 255) : ColorRGBA::green(),
                                                   {},
                                                   tcomp.transform.getRotation().getEulerAngles().z);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            ren2d.renderPresent();
        }
    }

    void CanvasRenderSystem::onComponentCreate(const EntityHandle &entity, const std::any &component) {
        if (component.type() == typeid(SpriteComponent)) {
            auto *t = std::any_cast<SpriteComponent>(&component);
            createTexture(entity, *t);
        } else if (component.type() == typeid(TextComponent)) {
            auto *t = std::any_cast<TextComponent>(&component);
            createText(entity, *t);
        }
    }

    void CanvasRenderSystem::onComponentDestroy(const EntityHandle &entity, const std::any &component) {
        if (component.type() == typeid(SpriteComponent)) {
            spriteTextures.erase(entity);
        } else if (component.type() == typeid(TextComponent)) {
            renderedTexts.erase(entity);
        }
    }

    void CanvasRenderSystem::onComponentUpdate(const EntityHandle &entity,
                                               const std::any &oldComponent,
                                               const std::any &newComponent) {
        if (oldComponent.type() == typeid(SpriteComponent)) {
            auto *os = std::any_cast<SpriteComponent>(&oldComponent);
            auto *ns = std::any_cast<SpriteComponent>(&newComponent);
            if (os->sprite != ns->sprite) {
                spriteTextures.erase(entity);
                createTexture(entity, *ns);
            }
        } else if (oldComponent.type() == typeid(TextComponent)) {
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