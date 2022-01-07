/**
 *  Mana - 3D Game Engine
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

#include "compat/renderwidgetqt.hpp"

#include "platform/graphics/opengl-qt/qtoglrenderdevice.hpp"
#include "platform/graphics/opengl-qt/qtoglrendertarget.hpp"

namespace engine {
    RenderWidgetQt::RenderWidgetQt(QWidget *parent, AssetManager &assetManager)
            : QOpenGLWidget(parent), assetManager(assetManager) {}

    std::unique_ptr<RenderTarget> RenderWidgetQt::getWidgetRenderTarget() {
            return std::move(std::make_unique<opengl::QtOGLRenderTarget>(defaultFramebufferObject(),
                                                                         Vec2i(frameSize().rwidth(), frameSize().rheight()),
                                                                         1,
                                                                         false));
    }
}