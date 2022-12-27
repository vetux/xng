/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include <sstream>

#include "xng/ecs/systems/canvasrendersystem.hpp"

#include "xng/ecs/components/transformcomponent.hpp"
#include "xng/ecs/components/canvastransformcomponent.hpp"
#include "xng/ecs/components/canvascomponent.hpp"
#include "xng/ecs/components/textcomponent.hpp"

#include "xng/ecs/components/rigidbodycomponent.hpp"

namespace xng {
    CanvasRenderSystem::CanvasRenderSystem(Renderer2D &renderer2D,
                                           RenderTarget &target,
                                           FontDriver &fontDriver)
            : ren2d(renderer2D), target(target), fontDriver(fontDriver) {}

    void CanvasRenderSystem::start(EntityScene &scene, EventBus &eventBus) {
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

    void CanvasRenderSystem::stop(EntityScene &scene, EventBus &eventBus) {
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

    void CanvasRenderSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        std::map<int, std::set<EntityHandle>> canvases;

        for (auto &p: scene.getPool<CanvasComponent>()) {
            if (!p.second.enabled)
                continue;
            canvases[p.second.layer].insert(p.first);
        }

        std::map<std::string, std::map<int, std::vector<Pass>>> passes;

        for (auto &p: scene.getPool<CanvasTransformComponent>()) {
            if (!p.second.enabled)
                continue;
            if (p.second.canvas.empty())
                continue;

            auto canvas = scene.getEntityByName(p.second.canvas);
            auto &c = scene.getComponent<CanvasComponent>(canvas);
            if (!c.enabled) {
                continue;
            }

            if (scene.checkComponent<SpriteComponent>(p.first)) {
                auto &r = scene.getComponent<SpriteComponent>(p.first);
                passes[p.second.canvas][r.layer].emplace_back(Pass{Pass::SPRITE, p.first});
            }

            if (scene.checkComponent<TextComponent>(p.first)) {
                auto &r = scene.getComponent<TextComponent>(p.first);
                passes[p.second.canvas][r.layer].emplace_back(Pass{Pass::TEXT, p.first});
            }
        }

        for (auto &canvasLayer: canvases) {
            for (auto &canvasId: canvasLayer.second) {
                auto &canvas = scene.getComponent<CanvasComponent>(canvasId);

                auto viewport = canvas.getViewport(target.getDescription().size);

                if (viewport.first.magnitude() > 0
                    || viewport.second.magnitude() > 0) {
                    ren2d.renderBegin(target,
                                      canvas.clear,
                                      canvas.clearColor,
                                      viewport.second,
                                      viewport.first);
                } else {
                    ren2d.renderBegin(target, canvas.clear, canvas.clearColor);
                }

                ren2d.setCameraPosition(canvas.cameraPosition);
                if (canvas.projectionSize.magnitude() > 0) {
                    ren2d.setProjection(Rectf({}, canvas.projectionSize));
                }

                if (scene.entityHasName(canvasId)) {
                    auto canvasName = scene.getEntityName(canvasId);
                    for (auto &pair: passes[canvasName]) {
                        for (auto &pass: pair.second) {
                            auto &rt = scene.getComponent<CanvasTransformComponent>(pass.ent);

                            Rectf dstRect;
                            Vec2f center;
                            float rotation = rt.rotation;

                            Transform worldTransform;

                            if (scene.checkComponent<TransformComponent>(pass.ent)) {
                                worldTransform = TransformComponent::walkHierarchy(
                                        scene.getComponent<TransformComponent>(pass.ent),
                                        scene);
                            }

                            switch (pass.type) {
                                case Pass::SPRITE: {
                                    auto &comp = scene.getComponent<SpriteComponent>(pass.ent);
                                    if (comp.sprite.assigned()) {
                                        dstRect = Rectf(rt.rect.position +
                                                        rt.getOffset(scene, target.getDescription().size),
                                                        rt.rect.dimensions);
                                        center = rt.center;

                                        dstRect.position.x -= worldTransform.getPosition().x;
                                        dstRect.position.y -= worldTransform.getPosition().y;
                                        rotation += worldTransform.getRotation().getEulerAngles().z;

                                        if (comp.mixSprite.assigned()) {
                                            ren2d.draw(Rectf({}, spriteTextures.at(
                                                               pass.ent).get()->getDescription().size.convert<float>()),
                                                       dstRect,
                                                       *spriteTextures.at(pass.ent),
                                                       *spriteTexturesB.at(pass.ent),
                                                       comp.mix,
                                                       center,
                                                       rotation,
                                                       comp.flipSprite);
                                        } else {
                                            ren2d.draw(Rectf({}, spriteTextures.at(
                                                               pass.ent).get()->getDescription().size.convert<float>()),
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
                                    auto &tcomp = scene.getComponent<TextComponent>(pass.ent);
                                    if (!tcomp.text.empty()
                                        && tcomp.font.assigned()) {
                                        auto texSize = renderedTexts.at(
                                                pass.ent).getTexture().getDescription().size.convert<float>();

                                        Vec2f displaySize(0);
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

                                        Vec2f displayOffset = rt.getOffset(scene, target.getDescription().size)
                                                              - rt.center;

                                        dstRect = Rectf(rt.rect.position + displayOffset,
                                                        displaySize);
                                        center = {};
                                        auto diff = rt.rect.dimensions - displaySize;
                                        switch (tcomp.textAnchor) {
                                            case CanvasTransformComponent::TOP_LEFT:
                                                break;
                                            case CanvasTransformComponent::LEFT:
                                                dstRect.position.y += diff.y / 2;
                                                break;
                                            case CanvasTransformComponent::BOTTOM_LEFT:
                                                dstRect.position.y += diff.y;
                                                break;
                                            case CanvasTransformComponent::TOP_CENTER:
                                                dstRect.position.x += diff.x / 2;
                                                break;
                                            case CanvasTransformComponent::CENTER:
                                                dstRect.position.x += diff.x / 2;
                                                dstRect.position.y += diff.y / 2;
                                                break;
                                            case CanvasTransformComponent::BOTTOM_CENTER:
                                                dstRect.position.x += diff.x / 2;
                                                dstRect.position.y += diff.y;
                                                break;
                                            case CanvasTransformComponent::TOP_RIGHT:
                                                dstRect.position.x += diff.x;
                                                break;
                                            case CanvasTransformComponent::RIGHT:
                                                dstRect.position.x += diff.x;
                                                dstRect.position.y += diff.y / 2;
                                                break;
                                            case CanvasTransformComponent::BOTTOM_RIGHT:
                                                dstRect.position.x += diff.x;
                                                dstRect.position.y += diff.y;
                                                break;
                                        }

                                        dstRect.position.x -= worldTransform.getPosition().x;
                                        dstRect.position.y -= worldTransform.getPosition().y;
                                        rotation += worldTransform.getRotation().getEulerAngles().z;

                                        Rectf srcRect;
                                        srcRect.position = tcomp.textScroll * texSize;
                                        srcRect.dimensions = dstRect.dimensions;

                                        ren2d.draw(renderedTexts.at(pass.ent),
                                                   srcRect,
                                                   dstRect,
                                                   tcomp.textColor,
                                                   center,
                                                   rotation);
                                    }
                                    break;
                                }
                            }

                            if (drawDebugGeometry) {
                                ren2d.draw(dstRect, ColorRGBA::blue(), false, center, rotation);

                                Rectf rtRect = {rt.rect.position + rt.getOffset(scene, target.getDescription().size)
                                                - Vec2f(worldTransform.getPosition().x, worldTransform.getPosition().y),
                                                rt.rect.dimensions};
                                ren2d.draw(rtRect, ColorRGBA::yellow(), false, rt.center, rotation);

                                auto dCenter = dstRect.position;
                                const int len = 20;
                                ren2d.draw(dCenter + Vec2f(-len, 0), dCenter + Vec2f(len, 0), ColorRGBA::red());
                                ren2d.draw(dCenter + Vec2f(0, len), dCenter + Vec2f(0, -len), ColorRGBA::red());

                                if (scene.checkComponent<RigidBodyComponent>(pass.ent)
                                    && scene.checkComponent<TransformComponent>(pass.ent)) {
                                    auto &rb = scene.getComponent<RigidBodyComponent>(pass.ent);
                                    auto &tcomp = scene.getComponent<TransformComponent>(pass.ent);
                                    for (auto &handle: rb.colliders) {
                                        auto &col = handle.get();
                                        if (col.shape.type == COLLIDER_2D
                                            && !col.shape.vertices.empty()) {
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

    void CanvasRenderSystem::onComponentCreate(const EntityHandle &entity, const Component &component) {
        if (component.getType() == typeid(SpriteComponent)) {
            const auto &t = dynamic_cast<const SpriteComponent &>(component);
            createTexture(entity, t);
        } else if (component.getType() == typeid(TextComponent)) {
            const auto &t = dynamic_cast<const TextComponent &>(component);
            createText(entity, t);
        }
    }

    void CanvasRenderSystem::onComponentDestroy(const EntityHandle &entity, const Component &component) {
        if (component.getType() == typeid(SpriteComponent)) {
            spriteTextures.erase(entity);
            spriteTexturesB.erase(entity);
        } else if (component.getType() == typeid(TextComponent)) {
            renderedTexts.erase(entity);
        }
    }

    void CanvasRenderSystem::onComponentUpdate(const EntityHandle &entity,
                                               const Component &oldComponent,
                                               const Component &newComponent) {
        if (oldComponent.getType() == typeid(SpriteComponent)) {
            auto &os = dynamic_cast<const SpriteComponent &>(oldComponent);
            auto &ns = dynamic_cast<const SpriteComponent &>(newComponent);
            if (os.sprite != ns.sprite || os.mixSprite != ns.mixSprite) {
                spriteTextures.erase(entity);
                spriteTexturesB.erase(entity);
                createTexture(entity, ns);
            }
        } else if (oldComponent.getType() == typeid(TextComponent)) {
            auto &ns = dynamic_cast<const TextComponent &>(newComponent);
            renderedTexts.erase(entity);
            createText(entity, ns);
        }
    }

    void CanvasRenderSystem::createTexture(const EntityHandle &ent, const SpriteComponent &t) {
        if (t.sprite.assigned()) {
            Vec2i dimensions = t.sprite.get().offset.dimensions;
            if (dimensions.x * dimensions.y == 0) {
                dimensions = t.sprite.get().image.get().getSize();
            }
            TextureBufferDesc desc;
            desc.size = dimensions;
            spriteTextures[ent] = ren2d.getDevice().createTextureBuffer(desc);
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
        if (t.mixSprite.assigned()) {
            Vec2i dimensions = t.mixSprite.get().offset.dimensions;
            if (dimensions.x * dimensions.y == 0) {
                dimensions = t.mixSprite.get().image.get().getSize();
            }
            TextureBufferDesc desc;
            desc.size = dimensions;
            spriteTexturesB[ent] = ren2d.getDevice().createTextureBuffer(desc);
            auto &img = t.mixSprite.get().image.get();
            if (img.getSize() != dimensions) {
                // Upload a slice of an image
                auto slice = img.slice(t.mixSprite.get().offset);
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
            renderedTexts.insert(
                    std::make_pair(ent, textRenderers.at(ent).render(comp.text, TextRenderProperties{comp.lineHeight,
                                                                                                     comp.lineWidth,
                                                                                                     comp.lineSpacing,
                                                                                                     comp.alignment})));
        }
    }
}