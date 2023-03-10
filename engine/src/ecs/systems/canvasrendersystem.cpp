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

#include <sstream>

#include "xng/ecs/systems/canvasrendersystem.hpp"

#include "xng/ecs/components/transformcomponent.hpp"
#include "xng/ecs/components/recttransformcomponent.hpp"
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
            if (spriteTextureHandles.find(pair.first) == spriteTextureHandles.end()) {
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
        for (auto &pair: spriteTextureHandles) {
            ren2d.destroyTexture(pair.second);
        }
        spriteTextureHandles.clear();
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

    static std::vector<EntityHandle> getDrawingOrderRecursive(EntityHandle rectComp,
                                                              std::map<EntityHandle, std::set<int>> rects) {
        std::vector<EntityHandle> ret;
        ret.emplace_back(rectComp);
        for (auto &id: rects[rectComp]) {
            auto order = getDrawingOrderRecursive(EntityHandle(id), rects);
            ret.insert(ret.begin(), order.begin(), order.end());
        }
        return ret;
    }

    void CanvasRenderSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        std::map<int, std::set<EntityHandle>> canvases;

        for (auto &p: scene.getPool<CanvasComponent>()) {
            if (!p.second.enabled)
                continue;
            canvases[p.second.layer].insert(p.first);
        }

        std::map<int, RectTransform> transforms;
        std::map<int, int> transformParents;
        std::map<EntityHandle, std::set<int>> rects;

        for (auto &comp: scene.getPool<RectTransformComponent>()) {
            if (!comp.second.parent.empty()
                && scene.entityNameExists(comp.second.parent)) {
                transforms[comp.first.id] = comp.second.rectTransform;
                auto parentEnt = scene.getEntityByName(comp.second.parent);

                if (scene.checkComponent<TextComponent>(EntityHandle(comp.first.id))) {
                    // Entities which contain a text component will get their rect size will be determined by the size of the rendered text.
                    // The text can then be lain out by using the rect transform parenting alignment.
                    transforms[comp.first.id].size = renderedTexts.at(
                            EntityHandle(comp.first.id)).getImage().getSize().convert<float>();
                }

                if (!scene.checkComponent<CanvasComponent>(parentEnt)) {
                    transformParents[comp.first.id] = parentEnt.id;
                } else {
                    // Apply alignment offset relative to the canvas
                    transforms[comp.first.id].position += RectTransform::getAlignmentOffset(
                            transforms[comp.first.id].size,
                            target.getDescription().size.convert<float>(),
                            transforms[comp.first.id].alignment);
                }

                rects[parentEnt].insert(comp.first.id);
            }
        }

        std::map<int, RectTransform> absoluteTransforms = RectTransform::getAbsolute(transforms, transformParents);

        for (auto &pair: canvases) {
            for (auto &canvasHandle: pair.second) {
                auto &canvasComponent = scene.getComponent<CanvasComponent>(canvasHandle);

                Vec2f targetSize = target.getDescription().size.convert<float>();
                if (canvasComponent.projection.dimensions.magnitude() > 0)
                    ren2d.renderBegin(target,
                                      canvasComponent.clear,
                                      canvasComponent.clearColor,
                                      {},
                                      target.getDescription().size,
                                      canvasComponent.cameraPosition,
                                      canvasComponent.projection);
                else
                    ren2d.renderBegin(target,
                                      canvasComponent.clear,
                                      canvasComponent.clearColor,
                                      {},
                                      target.getDescription().size,
                                      canvasComponent.cameraPosition);

                auto &transformIds = rects[canvasHandle];
                for (auto &id: transformIds) {
                    auto order = getDrawingOrderRecursive(EntityHandle(EntityHandle(id)), rects);
                    for (auto &handle: order) {
                        auto &transform = absoluteTransforms.at(handle.id);
                        if (scene.checkComponent<SpriteComponent>(handle)) {
                            auto &comp = scene.getComponent<SpriteComponent>(handle);
                            if (comp.enabled
                                && comp.sprite.assigned()) {
                                ren2d.draw(Rectf(comp.sprite.get().offset),
                                           Rectf(transform.position, transform.size),
                                           spriteTextureHandles.at(handle),
                                           transform.center,
                                           transform.rotation,
                                           comp.mix,
                                           comp.mixAlpha,
                                           comp.mixColor);
                            }
                        }
                        if (scene.checkComponent<TextComponent>(handle)) {
                            auto &comp = scene.getComponent<TextComponent>(handle);
                            auto &text = renderedTexts.at(handle);
                            auto dstPos = transform.position;
                            if (transformParents.find(handle.id) == transformParents.end()) {
                                // Apply alignment relative to the screen
                                dstPos += RectTransform::getAlignmentOffset(transform.size,
                                                                            targetSize,
                                                                            transform.alignment);
                            }
                            ren2d.draw(Rectf({}, text.getImage().getSize().convert<float>()),
                                       Rectf(dstPos, transform.size),
                                       textTextureHandles.at(handle),
                                       text.getImage().getSize().convert<float>() / 2,
                                       transform.rotation,
                                       0,
                                       0,
                                       {});
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
            if (spriteTextureHandles.find(entity) != spriteTextureHandles.end()) {
                ren2d.destroyTexture(spriteTextureHandles.at(entity));
                spriteTextureHandles.erase(entity);
            }
        } else if (component.getType() == typeid(TextComponent)) {
            renderedTexts.erase(entity);
            if (textTextureHandles.find(entity) != textTextureHandles.end()) {
                ren2d.destroyTexture(textTextureHandles.at(entity));
                textTextureHandles.erase(entity);
            }
        }
    }

    void CanvasRenderSystem::onComponentUpdate(const EntityHandle &entity,
                                               const Component &oldComponent,
                                               const Component &newComponent) {
        if (oldComponent.getType() == typeid(SpriteComponent)) {
            auto &os = dynamic_cast<const SpriteComponent &>(oldComponent);
            auto &ns = dynamic_cast<const SpriteComponent &>(newComponent);
            if (os.sprite != ns.sprite) {
                if (spriteTextureHandles.find(entity) != spriteTextureHandles.end()) {
                    ren2d.destroyTexture(spriteTextureHandles.at(entity));
                    spriteTextureHandles.erase(entity);
                }
                createTexture(entity, ns);
            }
        } else if (oldComponent.getType() == typeid(TextComponent)) {
            auto &ns = dynamic_cast<const TextComponent &>(newComponent);
            renderedTexts.erase(entity);
            if (textTextureHandles.find(entity) != textTextureHandles.end()) {
                ren2d.destroyTexture(textTextureHandles.at(entity));
                textTextureHandles.erase(entity);
            }
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
            auto &img = t.sprite.get().image.get();
            if (img.getSize() != dimensions) {
                // Upload a slice of an image
                auto slice = img.slice(t.sprite.get().offset);
                spriteTextureHandles[ent] = ren2d.createTexture(slice);
            } else {
                // Upload the whole image
                spriteTextureHandles[ent] = ren2d.createTexture(img);
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
                textRenderers.insert(std::move(std::make_pair(ent, TextRenderer(*fonts[comp.font.getUri()], ren2d))));
            }

            auto text = textRenderers.at(ent).render(comp.text, TextLayout{comp.lineHeight,
                                                                           comp.lineWidth,
                                                                           comp.lineSpacing,
                                                                           comp.alignment});
            renderedTexts.insert(std::make_pair(ent, text));
            textTextureHandles[ent] = ren2d.createTexture(text.getImage());
        }
    }

}