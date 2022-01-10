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

#ifndef XENGINE_QTOGLRENDERTARGET_HPP
#define XENGINE_QTOGLRENDERTARGET_HPP

#include "platform/graphics/rendertarget.hpp"

#include <QOpenGLFunctions_4_5_Core>

typedef unsigned int GLuint;

namespace xengine {
    namespace opengl {
        class QtOGLRenderTarget : public RenderTarget, public QOpenGLFunctions_4_5_Core {
        public:
            QtOGLRenderTarget(GLuint FBO, Vec2i size, int samples, bool delFBO);

            QtOGLRenderTarget(Vec2i size, int samples);

            ~QtOGLRenderTarget() override;

            Vec2i getSize() override;

            int getSamples() override;

            void blitColor(RenderTarget &source,
                           Vec2i sourceOffset,
                           Vec2i targetOffset,
                           Vec2i sourceRect,
                           Vec2i targetRect,
                           TextureBuffer::TextureFiltering filter,
                           int sourceIndex,
                           int targetIndex) override;

            void blitDepth(RenderTarget &source,
                           Vec2i sourceOffset,
                           Vec2i targetOffset,
                           Vec2i sourceRect,
                           Vec2i targetRect) override;

            void blitStencil(RenderTarget &source,
                             Vec2i sourceOffset,
                             Vec2i targetOffset,
                             Vec2i sourceRect,
                             Vec2i targetRect) override;

            void setNumberOfColorAttachments(int count) override;

            void attachColor(int index, TextureBuffer &texture) override;

            void attachDepth(TextureBuffer &texture) override;

            void attachStencil(TextureBuffer &texture) override;

            void attachDepthStencil(TextureBuffer &texture) override;

            void attachColor(int index, TextureBuffer::CubeMapFace face, TextureBuffer &texture) override;

            void attachDepth(TextureBuffer::CubeMapFace face, TextureBuffer &texture) override;

            void attachStencil(TextureBuffer::CubeMapFace face, TextureBuffer &texture) override;

            void attachDepthStencil(TextureBuffer::CubeMapFace face, TextureBuffer &texture) override;

            void detachColor(int index) override;

            void detachDepth() override;

            void detachStencil() override;

            void detachDepthStencil() override;

            bool isComplete() override;

            GLuint getFBO() const { return FBO; }

            void setFBO(GLuint value) { FBO = value; }

        protected:
            bool delFBO = true;
            GLuint FBO;
            GLuint colorRBO;
            GLuint depthStencilRBO;
            Vec2i size;
            int samples;
        };
    }
}

#endif //XENGINE_QTOGLRENDERTARGET_HPP
