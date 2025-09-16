/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include "xng/gui/canvasscaler.hpp"
#include "xng/gui/layoutengine.hpp"
#include "xng/util/time.hpp"

//TODO: Reimplement CanvasRenderSystem

namespace xng {
    CanvasRenderSystem::CanvasRenderSystem(Renderer2D &renderer2D,
                                           FontEngine &fontEngine,
                                           bool drawDebugGeometry,
                                           int pixelsPerMeter)
            : ren2d(renderer2D),
              fontEngine(fontEngine),
              drawDebugGeometry(drawDebugGeometry),
              pixelToMeter(1.0f / static_cast<float>(pixelsPerMeter)) {}

    void CanvasRenderSystem::start(EntityScene &scene, EventBus &eventBus) {
        scene.addListener(*this);
    }

    void CanvasRenderSystem::stop(EntityScene &scene, EventBus &eventBus) {
        scene.removeListener(*this);
        spriteTextures.clear();
        textPixelSizes.clear();
        textRenderers.clear();
        renderedTexts.clear();
        fontRenderers.clear();
    }

    static std::vector<EntityHandle> getDrawingOrderRecursive(EntityHandle rectComp,
                                                              std::map<EntityHandle, std::set<int>> rects) {
        std::vector<EntityHandle> ret;
        ret.emplace_back(rectComp);
        for (auto &id: rects[rectComp]) {
            auto order = getDrawingOrderRecursive(EntityHandle(id), rects);
            ret.insert(ret.end(), order.begin(), order.end());
        }
        return ret;
    }

    static Vec2f clampSize(const Vec2f &size, const Vec2f &limit) {
        if (size.x > limit.x && size.y > limit.y) {
            auto diffX = size.x - limit.x;
            auto diffY = size.y - limit.y;
            if (diffX > diffY) {
                return {limit.x, size.y * (limit.x / size.x)};
            } else {
                return {size.x * (limit.y / size.y), limit.y};
            }
        } else if (size.x > limit.x) {
            return {limit.x, size.y * (limit.x / size.x)};
        } else if (size.y > limit.y) {
            return {size.x * (limit.y / size.y), limit.y};
        } else {
            return size;
        }
    }

    void CanvasRenderSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        for (auto &pair: scene.getPool<SpriteComponent>()) {
            if (!pair.second.sprite.assigned()
                || !pair.second.sprite.isLoaded()
                || !pair.second.sprite.get().image.isLoaded())
                continue;
            if (spriteTextures.find(pair.first) == spriteTextures.end()) {
                createTexture(pair.first, pair.second);
            }
        }

        std::map<int, std::set<EntityHandle>> canvases;

        for (auto &p: scene.getPool<CanvasComponent>()) {
            if (!p.second.enabled)
                continue;
            canvases[p.second.layer].insert(p.first);
        }

        std::map<int, RectTransform> transforms;
        std::map<int, int> transformParents;
        std::map<EntityHandle, std::set<int>> parentsChildrenMapping;

        for (auto &comp: scene.getPool<RectTransformComponent>()) {
            if (!scene.checkComponent<TransformComponent>(comp.first)) {
                continue;
            }

            auto &t = scene.getComponent<TransformComponent>(comp.first);

            if (scene.checkComponent<CanvasComponent>(comp.first)) {
                auto &canvas = scene.getComponent<CanvasComponent>(comp.first);
                transforms[comp.first.id] = comp.second.rectTransform;
                //transforms[comp.first.id].size = target.getDescription().size.convert<float>();
            } else {
                if (t.parent.empty()
                    || !scene.entityNameExists(t.parent)
                    || !scene.checkComponent<TransformComponent>(scene.getEntityByName(t.parent))
                    || !scene.checkComponent<RectTransformComponent>(scene.getEntityByName(t.parent))) {
                    continue;
                }

                transforms[comp.first.id] = comp.second.rectTransform;

                auto parentEnt = scene.getEntityByName(t.parent);

                transformParents[comp.first.id] = parentEnt.id;

                parentsChildrenMapping[parentEnt].insert(comp.first.id);
            }

            transforms[comp.first.id].position.x += t.transform.getPosition().x / pixelToMeter;
            transforms[comp.first.id].position.y += t.transform.getPosition().y / pixelToMeter;
            transforms[comp.first.id].rotation += t.transform.getRotation().getEulerAngles().z;
            transforms[comp.first.id].size.x *= t.transform.getScale().x;
            transforms[comp.first.id].size.y *= t.transform.getScale().y;
        }

        for (auto &pair: canvases) {
            for (auto &canvasHandle: pair.second) {
                auto &canvasComponent = scene.getComponent<CanvasComponent>(canvasHandle);

                std::map<int, RectTransform> absoluteTransforms;
                switch (canvasComponent.scaleMode) {
                    case SCALE_REFERENCE_RESOLUTION:
                    /*    absoluteTransforms = LayoutEngine::getAbsoluteReferenceScaled(transforms,
                                                                                      transformParents,
                                                                                      canvasComponent.referenceResolution,
                                                                                      target.getDescription().size.convert<float>(),
                                                                                      canvasComponent.referenceFitWidth);*/
                        break;
                    default:
                        absoluteTransforms = LayoutEngine::getAbsolute(transforms, transformParents);
                        break;
                }

                auto &transformIds = parentsChildrenMapping[canvasHandle];

                for (auto &id: transformIds) {
                    auto order = getDrawingOrderRecursive(EntityHandle(EntityHandle(id)), parentsChildrenMapping);
                    for (auto &handle: order) {
                        auto transform = absoluteTransforms.at(handle.id);
                        if (scene.checkComponent<TextComponent>(handle)) {
                            auto &comp = scene.getComponent<TextComponent>(handle);
                            auto scale = transform.size / transforms.at(handle.id).size;
                            updateText(handle, comp, scale);
                        }
                    }
                }

             /*   ren2d.renderBegin(target,
                                  canvasComponent.clear,
                                  canvasComponent.clearColor,
                                  {},
                                  target.getDescription().size,
                                  canvasComponent.cameraPosition);*/

                for (auto &id: transformIds) {
                    auto order = getDrawingOrderRecursive(EntityHandle(EntityHandle(id)), parentsChildrenMapping);

                    for (auto &handle: order) {
                        auto transform = absoluteTransforms.at(handle.id);

                        if (scene.checkComponent<SpriteComponent>(handle)) {
                            auto &comp = scene.getComponent<SpriteComponent>(handle);
                            auto texIt = spriteTextures.find(handle);
                            if (texIt != spriteTextures.end()) {
                                ren2d.draw(Rectf(comp.sprite.get().offset),
                                           Rectf(transform.position, transform.size),
                                           texIt->second,
                                           transform.center,
                                           transform.rotation,
                                           comp.filter,
                                           comp.mix,
                                           comp.mixAlpha,
                                           comp.mixColor);
                            }
                            if (drawDebugGeometry) {
                                ren2d.draw(Rectf(transform.position, transform.size),
                                           ColorRGBA::yellow(),
                                           false,
                                           transform.center,
                                           transform.rotation);
                            }
                        } else if (scene.checkComponent<TextComponent>(handle)) {
                            auto debugTransform = transform;
                            auto &comp = scene.getComponent<TextComponent>(handle);

                            if (renderedTexts.find(handle) == renderedTexts.end())
                                continue;

                            auto &text = renderedTexts.at(handle);

                            transform.position += transform.size / 2;

                          /*  auto imgSize = text.getImage().getResolution().convert<float>();

                            Rectf srcRect({}, imgSize);

                            auto diff = transform.size - imgSize;
                            if (diff.x < 0) {
                                srcRect.dimensions.x = transform.size.x;
                            } else {
                                transform.size.x = imgSize.x;
                            }
                            if (diff.y < 0) {
                                srcRect.dimensions.y = transform.size.y;
                            } else {
                                transform.size.y = imgSize.y;
                            }

                            transform.position -= transform.size / 2;

                            ren2d.draw(srcRect,
                                       Rectf(transform.position, transform.size),
                                       textTextures.at(handle),
                                       transform.center,
                                       transform.rotation,
                                       comp.filter,
                                       comp.textColor);*/
                            if (drawDebugGeometry) {
                                ren2d.draw(Rectf(debugTransform.position, debugTransform.size),
                                           ColorRGBA::yellow(),
                                           false,
                                           debugTransform.center,
                                           debugTransform.rotation);
                                ren2d.draw(Rectf(transform.position, transform.size),
                                           ColorRGBA::red(),
                                           false,
                                           transform.center,
                                           transform.rotation);
                            }
                        } else {
                            if (drawDebugGeometry) {
                                ren2d.draw(Rectf(transform.position, transform.size),
                                           ColorRGBA::fuchsia(),
                                           false,
                                           transform.center,
                                           transform.rotation);
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
        } else if (component.getType() == typeid(TextComponent)) {
        }
    }

    void CanvasRenderSystem::onComponentDestroy(const EntityHandle &entity, const Component &component) {
        if (component.getType() == typeid(SpriteComponent)) {
            if (spriteTextures.find(entity) != spriteTextures.end()) {
                spriteTextures.erase(entity);
            }
        } else if (component.getType() == typeid(TextComponent)) {
            renderedTexts.erase(entity);
            if (textTextures.find(entity) != textTextures.end()) {
                textTextures.erase(entity);
            }
            if (textPixelSizes.find(entity) != textPixelSizes.end()) {
                auto v = textPixelSizes.at(entity);
                textPixelSizes.erase(entity);
                destroyTextRenderer(v);
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
                if (spriteTextures.find(entity) != spriteTextures.end()) {
                    spriteTextures.erase(entity);
                }
            }
        } else if (oldComponent.getType() == typeid(TextComponent)) {
            auto &os = dynamic_cast<const TextComponent &>(oldComponent);
            auto &ns = dynamic_cast<const TextComponent &>(newComponent);
            if (os != ns) {
                renderedTexts.erase(entity);
                if (textTextures.find(entity) != textTextures.end()) {
                    textTextures.erase(entity);
                }
            }
        }
    }

    void CanvasRenderSystem::createTexture(const EntityHandle &ent, const SpriteComponent &t) {
        if (t.sprite.assigned()) {
            Vec2i dimensions = t.sprite.get().offset.dimensions;
            if (dimensions.x * dimensions.y == 0) {
                dimensions = t.sprite.get().image.get().getResolution();
            }
           /* TextureBufferDesc desc;
            desc.size = dimensions;
            auto &img = t.sprite.get().image.get();
            if (img.getResolution() != dimensions) {
                // Upload a slice of an image
                auto slice = img.slice(t.sprite.get().offset);
                spriteTextures[ent] = ren2d.createTexture(slice);
            } else {
                // Upload the whole image
                spriteTextures[ent] = ren2d.createTexture(img);
            }*/
        }
    }

    void CanvasRenderSystem::updateText(const EntityHandle &ent, const TextComponent &comp, const Vec2f &sizeScale) {
        if (!comp.text.empty()
            && comp.font.assigned()
            && comp.font.isLoaded()) {
            auto pointSize = (comp.pixelSize.convert<float>() * sizeScale).convert<int>();

            auto it = textPixelSizes.find(ent);
            if (it == textPixelSizes.end()
                || pointSize != textPixelSizes.at(ent)) {
                if (it != textPixelSizes.end()) {
                    auto v = textPixelSizes.at(ent);
                    textPixelSizes.erase(ent);
                    destroyTextRenderer(v);
                }
                textPixelSizes[ent] = pointSize;

                auto eIt = fontRenderers.find(comp.font.getUri());
                if (eIt == fontRenderers.end()) {
                    fontRenderers[comp.font.getUri()] = fontEngine.createFontRenderer(comp.font.get());
                }

                auto rIt = textRenderers.find(pointSize);
                if (rIt == textRenderers.end()) {
              /*      textRenderers.insert(std::move(
                            std::make_pair(pointSize,
                                           std::move(TextRenderer(*fontRenderers[comp.font.getUri()], ren2d, pointSize)))));*/
                }

             /*   auto text = textRenderers.at(pointSize).render(comp.text, TextLayout{
                        comp.lineHeight == 0 ? pointSize.y : static_cast<int>(static_cast<float>(comp.lineHeight) *
                                                                              sizeScale.y),
                        static_cast<int>(static_cast<float>(comp.lineWidth)),
                        static_cast<int>(static_cast<float>(comp.lineSpacing)),
                        comp.alignment});
                renderedTexts[ent] = text;
                textTextures[ent] = ren2d.createTexture(text.getImage());*/
            }
        }
    }

    void CanvasRenderSystem::destroyTextRenderer(const Vec2i &size) {
        bool delText = true;
        for (auto &pair: textPixelSizes) {
            if (pair.second == size) {
                delText = false;
                break;
            }
        }
        if (delText) {
            textRenderers.erase(size);
        }
    }
}