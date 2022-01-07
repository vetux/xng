/**
 *  XEngine - C++ game engine library
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

#ifndef XENGINE_RENDERWIDGETQT_HPP
#define XENGINE_RENDERWIDGETQT_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include "platform/graphics/rendertarget.hpp"
#include "render/deferred/deferredrenderer.hpp"

#include "render/deferred/passes/skyboxpass.hpp"
#include "render/deferred/passes/prepass.hpp"
#include "render/deferred/passes/phongshadepass.hpp"

namespace xengine {
    class XENGINE_EXPORT RenderWidgetQt : public QOpenGLWidget {
    public:
        RenderWidgetQt(QWidget *parent, AssetManager &assetManager);

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
            ren = std::make_unique<DeferredRenderer>(*renderDevice, *assetRenderManager);
            ren->addRenderPass(std::make_unique<SkyboxPass>(*renderDevice));
            ren->addRenderPass(std::make_unique<PrePass>(*renderDevice));
            ren->addRenderPass(std::make_unique<PhongShadePass>(*renderDevice));
            ren->getCompositor().setLayers({Compositor::Layer("Skybox", SkyboxPass::COLOR, ""),
                                            Compositor::Layer("Phong", PhongShadePass::COMBINED, "")});
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
            ren->render(*target, scene);
        }

    private:
        std::unique_ptr<RenderTarget> getWidgetRenderTarget();

        AssetManager &assetManager;

        Scene scene;

        std::unique_ptr<RenderDevice> renderDevice;
        std::unique_ptr<AssetRenderManager> assetRenderManager;
        std::unique_ptr<DeferredRenderer> ren;
    };
}

#endif //XENGINE_RENDERWIDGETQT_HPP
