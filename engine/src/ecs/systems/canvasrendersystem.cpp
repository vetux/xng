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
#include "ecs/components/canvastransformcomponent.hpp"
#include "ecs/components/canvascomponent.hpp"
#include "ecs/components/textcomponent.hpp"

#include "ecs/components/rigidbodycomponent.hpp"

namespace xng {
    CanvasRenderSystem::CanvasRenderSystem(Renderer2D &renderer2D,
                                           RenderTarget &target,
                                           FontDriver &fontDriver)
            : ren2d(renderer2D), target(target), fontDriver(fontDriver) {}

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
        std::map<int, std::set<std::string>> canvases;
        std::map<std::string, std::map<int, std::vector<Pass>>> passes;

        for (auto &p: scene.getPool<CanvasTransformComponent>()) {
            if (!p.second.enabled)
                continue;
            auto canvas = scene.getEntityByName(p.second.canvas);
            auto &c = scene.lookup<CanvasComponent>(canvas);
            if (!c.enabled) {
                continue;
            }

            if (scene.check<SpriteComponent>(p.first)) {
                auto &r = scene.lookup<SpriteComponent>(p.first);
                passes[p.second.canvas][r.layer].emplace_back(Pass{Pass::SPRITE, p.first});
                canvases[c.layer].insert(p.second.canvas);
            }

            if (scene.check<TextComponent>(p.first)) {
                auto &r = scene.lookup<TextComponent>(p.first);
                passes[p.second.canvas][r.layer].emplace_back(Pass{Pass::TEXT, p.first});
                canvases[c.layer].insert(p.second.canvas);
            }
        }

        for (auto &canvasLayer: canvases) {
            for (auto &canvasName: canvasLayer.second) {
                auto canvasEnt = scene.getEntityByName(canvasName);
                auto canvas = scene.lookup<CanvasComponent>(canvasEnt);

                if (canvas.lockAspectRatio) {
                    canvas.fitProjectionToScreen(target.getDescription().size);
                    scene.updateComponent(canvasEnt, canvas);
                }

                if (canvas.viewportSize.magnitude() > 0
                    || canvas.viewportOffset.magnitude() > 0) {
                    ren2d.renderBegin(target,
                                      canvas.clear,
                                      canvas.clearColor,
                                      canvas.viewportOffset,
                                      canvas.viewportSize);
                } else {
                    ren2d.renderBegin(target, canvas.clear, canvas.clearColor);
                }

                ren2d.setCameraPosition(canvas.cameraPosition);
                if (canvas.projectionSize.magnitude() > 0) {
                    ren2d.setProjection(Rectf({}, canvas.projectionSize));
                }

                for (auto &pair: passes.at(canvasName)) {
                    for (auto &pass: pair.second) {
                        auto &rt = scene.lookup<CanvasTransformComponent>(pass.ent);

                        Rectf dstRect;
                        Vec2f center;
                        float rotation = rt.rotation;

                        Transform worldTransform;

                        if (scene.check<TransformComponent>(pass.ent)) {
                            worldTransform = TransformComponent::walkHierarchy(
                                    scene.lookup<TransformComponent>(pass.ent),
                                    scene);
                        }

                        switch (pass.type) {
                            case Pass::SPRITE: {
                                auto &comp = scene.lookup<SpriteComponent>(pass.ent);
                                if (comp.sprite.assigned()) {
                                    dstRect = Rectf(rt.rect.position +
                                                    rt.getOffset(scene, target.getDescription().size.convert<float>()),
                                                    rt.rect.dimensions);
                                    center = rt.center;

                                    dstRect.position.x -= worldTransform.getPosition().x;
                                    dstRect.position.y -= worldTransform.getPosition().y;
                                    rotation += worldTransform.getRotation().getEulerAngles().z;

                                    if (comp.spriteB.assigned()) {
                                        ren2d.draw(Rectf({}, dstRect.dimensions),
                                                   dstRect,
                                                   *spriteTextures.at(pass.ent),
                                                   *spriteTexturesB.at(pass.ent),
                                                   comp.blendScale,
                                                   center,
                                                   rotation,
                                                   comp.flipSprite);
                                    } else {
                                        ren2d.draw(Rectf({}, dstRect.dimensions),
                                                   dstRect,
                                                   *spriteTextures.at(pass.ent),
                                                   rt.center,
                                                   rotation,
                                                   comp.flipSprite,
                                                   comp.mix,
                                                   comp.mixColor);
                                    }
                                }
                                break;
                            }
                            case Pass::TEXT: {
                                auto &tcomp = scene.lookup<TextComponent>(pass.ent);
                                if (!tcomp.text.empty()
                                && tcomp.font.assigned()) {
                                    auto texSize = renderedTexts.at(
                                            pass.ent).getTexture().getDescription().size.convert<float>();
                                    Vec2f displaySize(0);
                                    Vec2f displayOffset = rt.getOffset(scene,
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

                                    dstRect.position.x -= worldTransform.getPosition().x;
                                    dstRect.position.y -= worldTransform.getPosition().y;
                                    rotation += worldTransform.getRotation().getEulerAngles().z;

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
            spriteTexturesB.erase(entity);
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
            if (os->sprite != ns->sprite || os->spriteB != ns->spriteB) {
                spriteTextures.erase(entity);
                spriteTexturesB.erase(entity);
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
        if (t.spriteB.assigned()) {
            Vec2i dimensions = t.spriteB.get().offset.dimensions;
            if (dimensions.x * dimensions.y == 0) {
                dimensions = t.spriteB.get().image.get().getSize();
            }
            spriteTexturesB[ent] = ren2d.getDevice().createTextureBuffer({dimensions});
            auto &img = t.spriteB.get().image.get();
            if (img.getSize() != dimensions) {
                // Upload a slice of an image
                auto slice = img.slice(t.spriteB.get().offset);
                spriteTexturesB[ent]->upload(slice);
            } else {
                // Upload the whole image
                spriteTexturesB[ent]->upload(img);
            }
        }
    }

    void CanvasRenderSystem::createText(const EntityHandle &ent, const TextComponent &comp) {
        if (!comp.text.empty()
            && comp.font.assigned()) {
            auto it = fonts.find(comp.font.getUri());
            if (it == fonts.end()) {
                std::string str = std::string(comp.font.get().bytes.begin(), comp.font.get().bytes.end());
                auto stream = std::stringstream(str);
                fonts[comp.font.getUri()] = fontDriver.createFont(stream);
            }
            auto rIt = textRenderers.find(ent);
            if (rIt == textRenderers.end()) {
                fonts.at(comp.font.getUri())->setPixelSize(comp.pixelSize);
                textRenderers.insert(std::make_pair(ent, TextRenderer(*fonts[comp.font.getUri()], ren2d)));
            }
            renderedTexts.insert(std::make_pair(ent, textRenderers.at(ent).render(comp.text, comp.lineHeight,
                                                                                  comp.lineWidth,
                                                                                  comp.lineSpacing)));
        }
    }
}