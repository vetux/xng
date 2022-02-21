/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_QTRENDERWIDGET_HPP
#define XENGINE_QTRENDERWIDGET_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include "render/platform/rendertarget.hpp"
#include "render/deferred/deferredpipeline.hpp"

#include "render/deferred/passes/skyboxpass.hpp"
#include "render/deferred/passes/phongpass.hpp"

namespace xengine {
    class XENGINE_EXPORT QtRenderWidget : public QOpenGLWidget {
    public:
        /**
         * The implementation of this interface creates the pipeline when the QOpenGLWidget is initializing.
         * This is needed to be a delayed operation because QOpenGLWidget only allows usage of OpenGL calls at
         * specific points (initializeGL() ...)
         */
        class Allocator {
        public:
            /**
             * Create the pipeline.
             *
             * @param device
             * @param ren
             */
            virtual std::unique_ptr<Pipeline> createPipeline() = 0;
        };

        QtRenderWidget(QWidget *parent,
                       AssetManager &assetManager,
                       std::unique_ptr<Allocator> allocator)
                : QOpenGLWidget(parent), assetManager(assetManager), allocator(std::move(allocator)) {}

        void setScene(const Scene &s) {
            scene = s;
            update();
        }

    protected:
        void initializeGL() override {
            QOpenGLWidget::initializeGL();
            renderDevice = RenderDevice::create(OPENGL_4_6_QT);
            assetRenderManager = std::make_unique<AssetRenderManager>(assetManager,
                                                                      renderDevice->getAllocator());
            pipeline = allocator->createPipeline();
        }

        void resizeGL(int w, int h) override {
            QOpenGLWidget::resizeGL(w, h);
        }

        void paintGL() override {
            QOpenGLWidget::paintGL();
            std::unique_ptr<RenderTarget> target = getWidgetRenderTarget();
            if (!target->isComplete())
                return;
            scene.camera.aspectRatio = static_cast<float>( target->getSize().x)
                                       / static_cast<float>(target->getSize().y);
            pipeline->render(*target, scene);
        }

    private:
        std::unique_ptr<RenderTarget> getWidgetRenderTarget();

        AssetManager &assetManager;

        Scene scene;

        std::unique_ptr<Pipeline> pipeline;

        std::unique_ptr<RenderDevice> renderDevice;
        std::unique_ptr<AssetRenderManager> assetRenderManager;

        std::unique_ptr<Allocator> allocator;
    };
}

#endif //XENGINE_QTRENDERWIDGET_HPP
