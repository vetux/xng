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

#include "platform/graphics/rendertarget.hpp"
#include "render/deferred/deferredrenderer.hpp"

#include "render/deferred/passes/skyboxpass.hpp"
#include "render/deferred/passes/prepass.hpp"
#include "render/deferred/passes/phongshadepass.hpp"

namespace xengine {
    class XENGINE_EXPORT QtRenderWidget : public QOpenGLWidget {
    public:
        class Allocator {
        public:
            virtual void addPasses(RenderDevice &device, DeferredRenderer &ren) = 0;
        };

        QtRenderWidget(QWidget *parent,
                       AssetManager &assetManager,
                       std::unique_ptr<Allocator> allocator)
                : QOpenGLWidget(parent), assetManager(assetManager), allocator(std::move(allocator)) {}

        void setScene(const Scene &s) {
            scene = s;
            update();
        }

        void setLayers(const std::vector<Compositor::Layer> &layers) {
            renderLayers = layers;
        }

    protected:
        void initializeGL() override {
            QOpenGLWidget::initializeGL();
            renderDevice = RenderDevice::create(OPENGL_4_6_QT);
            assetRenderManager = std::make_unique<AssetRenderManager>(assetManager,
                                                                      renderDevice->getAllocator());
            ren = std::make_unique<DeferredRenderer>(*renderDevice, *assetRenderManager);
            allocator->addPasses(*renderDevice, *ren);
        }

        void resizeGL(int w, int h) override {
            QOpenGLWidget::resizeGL(w, h);
            ren->getGeometryBuffer().setSize({w, h});
        }

        void paintGL() override {
            QOpenGLWidget::paintGL();
            std::unique_ptr<RenderTarget> target = getWidgetRenderTarget();
            if (!target->isComplete())
                return;
            scene.camera.aspectRatio = static_cast<float>( target->getSize().x)
                                       / static_cast<float>(target->getSize().y);
            ren->getGeometryBuffer().setSize(target->getSize());
            ren->getCompositor().setLayers(renderLayers);
            ren->render(*target, scene);
        }

    private:
        std::unique_ptr<RenderTarget> getWidgetRenderTarget();

        AssetManager &assetManager;

        Scene scene;

        std::unique_ptr<RenderDevice> renderDevice;
        std::unique_ptr<AssetRenderManager> assetRenderManager;
        std::unique_ptr<DeferredRenderer> ren;

        std::unique_ptr<Allocator> allocator;
        std::vector<Compositor::Layer> renderLayers;
    };
}

#endif //XENGINE_QTRENDERWIDGET_HPP
