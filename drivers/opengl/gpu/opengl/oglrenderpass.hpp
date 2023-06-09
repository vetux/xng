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

#ifndef XENGINE_OGLRENDERPASS_HPP
#define XENGINE_OGLRENDERPASS_HPP

#include "xng/gpu/renderpass.hpp"

#include <utility>

namespace xng::opengl {
    class OGLRenderPass : public RenderPass {
    public:
        std::function<void(RenderObject * )> destructor;

        RenderPassDesc passDesc;

        OGLRenderPass(std::function<void(RenderObject * )> destructor,
                      RenderPassDesc passDesc)
                : destructor(std::move(destructor)),
                  passDesc(passDesc) {}

        ~OGLRenderPass() override {
            destructor(this);
        };

        const RenderPassDesc &getDescription() override {
            return passDesc;
        }
    };
}

#endif //XENGINE_OGLRENDERPASS_HPP
