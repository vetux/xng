/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_RENDERSCENE_HPP
#define XENGINE_RENDERSCENE_HPP

#include "xng/renderer/objects/rendertransform.hpp"
#include "xng/renderer/objects/rendermaterial.hpp"
#include "xng/renderer/objects/rendermesh.hpp"

#include "xng/renderer/pipeline/rendershader.hpp"
#include "xng/renderer/pipeline/renderbatch.hpp"

#include "xng/renderer/renderallocator.hpp"
#include "xng/renderer/renderpath.hpp"
#include "xng/renderer/shadingmodel.hpp"
#include "xng/renderer/camera.hpp"

namespace xng {
    /**
     * A scene should represent the complete contents of one frame.
     *
     * It is bounded and defines the available features for the renderer.
     */
    class RenderScene {
    public:
        typedef size_t ObjectID;

        class Model {
        public:
            Model(RenderObjectHandle<RenderTransform> transform,
                  RenderObjectHandle<RenderMaterial> material,
                  std::vector<RenderObjectHandle<RenderMesh> > meshes,
                  const RenderPath renderPath,
                  const bool castShadows,
                  const ShadingModel shadingModel,
                  const bool receiveShadows)
                : transform(std::move(transform)),
                  material(std::move(material)),
                  meshes(std::move(meshes)),
                  renderPath(renderPath),
                  castShadows(castShadows),
                  shadingModel(shadingModel),
                  receiveShadows(receiveShadows),
                  shader(nullptr) {
            }

            Model(RenderObjectHandle<RenderTransform> transform,
                  RenderObjectHandle<RenderMaterial> material,
                  std::vector<RenderObjectHandle<RenderMesh> > meshes,
                  const RenderPath renderPath,
                  const bool castShadows,
                  std::shared_ptr<RenderShader> shader)
                : transform(std::move(transform)),
                  material(std::move(material)),
                  meshes(std::move(meshes)),
                  renderPath(renderPath),
                  castShadows(castShadows),
                  shader(std::move(shader)) {
            }

            [[nodiscard]] RenderObjectHandle<RenderTransform> getTransform() const {
                return transform;
            }

            [[nodiscard]] RenderObjectHandle<RenderMaterial> getMaterial() const {
                return material;
            }

            [[nodiscard]] const std::vector<RenderObjectHandle<RenderMesh> > &getMeshes() const {
                return meshes;
            }

            [[nodiscard]] RenderPath getRenderPath() const {
                return renderPath;
            }

            [[nodiscard]] ShadingModel getShadingModel() const {
                return shadingModel;
            }

            [[nodiscard]] bool isCastShadows() const {
                return castShadows;
            }

            [[nodiscard]] bool isReceiveShadows() const {
                return receiveShadows;
            }

            [[nodiscard]] std::shared_ptr<RenderShader> getShader() const {
                return shader;
            }

        private:
            RenderObjectHandle<RenderTransform> transform;
            RenderObjectHandle<RenderMaterial> material;
            std::vector<RenderObjectHandle<RenderMesh> > meshes;

            RenderPath renderPath = RENDER_PATH_DEFERRED;
            bool castShadows = false;

            ShadingModel shadingModel = SHADING_MODEL_NONE;
            bool receiveShadows = false;

            std::shared_ptr<RenderShader> shader;
        };

        class Paint {
        public:
            static Mat4f getTransform(const Rectf &dstRect, const float rotation) {
                return MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                       * MatrixMath::rotate(Vec3f(0, 0, rotation))
                       * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1));
            }

            /**
             * Paint Line
             *
             * @param allocator
             * @param start
             * @param end
             * @param color
             * @param center
             * @param rotation
             */
            Paint(RenderAllocator &allocator,
                  const Vec2f &start,
                  const Vec2f &end,
                  const ColorRGBA &color,
                  const Vec2f &center = {},
                  const float rotation = 0) {
                transform = allocator.createTransform(MatrixMath::rotate(Vec3f(0, 0, rotation))
                                                      * MatrixMath::translate(Vec3f(center.x, center.y, 0)));
                material = allocator.createMaterial(color,
                                                    0,
                                                    0,
                                                    0,
                                                    0,
                                                    false,
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {});
                Mesh m;
                m.primitive = Mesh::TRIANGLES;
                m.positions.emplace_back(start.x, start.y, 1);
                m.positions.emplace_back(end.x, end.y, 1);
                m.positions.emplace_back(end.x, end.y, 1);
                mesh = allocator.createMesh(m, {});
            }

            /**
             * Paint Point
             *
             * @param allocator
             * @param unitQuadMesh
             * @param position
             * @param size
             * @param color
             */
            Paint(RenderAllocator &allocator,
                  RenderObjectHandle<RenderMesh> unitQuadMesh,
                  const Vec2f &position,
                  const float size,
                  const ColorRGBA &color) {
                transform = allocator.createTransform(MatrixMath::scale(Vec3f(size, size, 1))
                                                      * MatrixMath::translate(Vec3f(position.x, position.y, 0)));
                material = allocator.createMaterial(color,
                                                    0,
                                                    0,
                                                    0,
                                                    0,
                                                    false,
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {});
                mesh = std::move(unitQuadMesh);
            }

            /**
             * Paint Rect
             *
             * @param allocator
             * @param unitQuadMesh
             * @param dstRect
             * @param color
             * @param center
             * @param rotation
             */
            Paint(RenderAllocator &allocator,
                  RenderObjectHandle<RenderMesh> unitQuadMesh,
                  const Rectf &dstRect,
                  const ColorRGBA &color,
                  const Vec2f &center = {},
                  const float rotation = 0) {
                transform = allocator.createTransform(
                    MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                    * MatrixMath::rotate(Vec3f(0, 0, rotation))
                    * MatrixMath::translate(Vec3f(center.x, center.y, 0))
                    * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1)));
                material = allocator.createMaterial(color,
                                                    0,
                                                    0,
                                                    0,
                                                    0,
                                                    false,
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {});
                mesh = std::move(unitQuadMesh);
            }

            /**
             * Paint Texture
             *
             * @param allocator
             * @param unitQuadMesh
             * @param dstRect
             * @param texture
             * @param samplingProperties
             * @param center
             * @param rotation
             */
            Paint(RenderAllocator &allocator,
                  RenderObjectHandle<RenderMesh> unitQuadMesh,
                  const Rectf &dstRect,
                  const RenderObjectHandle<RenderTexture> &texture,
                  const SamplingProperties &samplingProperties,
                  const Vec2f &center = {},
                  const float rotation = 0) {
                transform = allocator.createTransform(
                    MatrixMath::translate(Vec3f(dstRect.position.x, dstRect.position.y, 0))
                    * MatrixMath::rotate(Vec3f(0, 0, rotation))
                    * MatrixMath::translate(Vec3f(center.x, center.y, 0))
                    * MatrixMath::scale(Vec3f(dstRect.dimensions.x, dstRect.dimensions.y, 1)));
                material = allocator.createMaterial({},
                                                    0,
                                                    0,
                                                    0,
                                                    0,
                                                    false,
                                                    texture,
                                                    samplingProperties,
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {},
                                                    {});
                mesh = std::move(unitQuadMesh);
            }

            [[nodiscard]] RenderObjectHandle<RenderTransform> getTransform() const {
                return transform;
            }

            [[nodiscard]] RenderObjectHandle<RenderMaterial> getMaterial() const {
                return material;
            }

            [[nodiscard]] RenderObjectHandle<RenderMesh> getMesh() const {
                return mesh;
            }

        private:
            RenderObjectHandle<RenderTransform> transform;
            RenderObjectHandle<RenderMaterial> material;
            RenderObjectHandle<RenderMesh> mesh;
        };

        class Canvas {
        public:
            /**
             * Construct a default local transformation matrix for translating the canvas contents in canvas space.
             *
             * E.g., for a pixel-perfect screen space canvas use left=0, right=screenSize.x, top=0, bottom=screenSize.y
             *
             * @param left
             * @param right
             * @param top
             * @param bottom
             */
            static Mat4f getLocalProjection(const float left, const float right, const float top, const float bottom) {
                Transform cameraTransform;
                cameraTransform.setPosition({0, 0, -1});

                const auto view = Camera::getView(cameraTransform);
                const auto projection = Camera::getOrthographicProjection(left,
                                                                          right,
                                                                          bottom,
                                                                          top,
                                                                          0.001,
                                                                          1);

                return projection * view;
            }

            Canvas(const Mat4f &localProjection,
                   RenderObjectHandle<RenderTexture> texture,
                   std::vector<Paint> paints)
                : localProjection(localProjection),
                  texture(std::move(texture)),
                  paints(std::move(paints)) {
            }

        private:
            Mat4f localProjection;
            RenderObjectHandle<RenderTexture> texture;
            std::vector<Paint> paints;
        };

        struct UserShadingBatch {
            std::shared_ptr<RenderShader> shader;
            std::shared_ptr<RenderBatch> batch;
        };

        struct CanvasBatch {
            RenderObjectHandle<RenderTexture> texture;
            std::shared_ptr<RenderBatch> batch;
        };

        // TODO: Design scene object interface.
        // Models and canvases should be a reactive interface so the renderer doesnt need to rebuild the draw list every frame
        ObjectID createModel(Model &&model);

        ObjectID createCanvas(Canvas &&canvas);

        void setLights(const std::vector<RenderObjectHandle<RenderPointLight> > &lights);

        void setCamera(const Camera &camera);

        const std::unordered_map<ObjectID, Model> &getModels() const;

        const std::unordered_map<ObjectID, Canvas> &getCanvases() const;

        std::shared_ptr<RenderBatch> getPbrDeferredBatch();

        std::shared_ptr<RenderBatch> getPbrForwardBatch();

        std::shared_ptr<RenderBatch> getShadowCastersBatch();

        std::vector<UserShadingBatch> getUserShadingBatches();

        std::shared_ptr<RenderBatch> getScreenCanvasBatch();

        std::vector<CanvasBatch> getCanvasBatches();

    private:
        std::unordered_map<ObjectID, Model> models;
        std::unordered_map<ObjectID, Canvas> canvases;

        RenderObjectHandle<RenderMesh> unitQuadMesh;
        RenderObjectHandle<RenderMesh> unitCubeMesh;
    };
}

#endif //XENGINE_RENDERSCENE_HPP
