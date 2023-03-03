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

#ifndef XENGINE_TESTPASS_HPP
#define XENGINE_TESTPASS_HPP

#include "xng/xng.hpp"

using namespace xng;

class TestPass : public FrameGraphPass {
public:
    SHARED_PROPERTY(TestPass, COLOR)

    void setup(FrameGraphBuilder &builder) override {
        gBufferPosition = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_POSITION);
        gBufferNormal = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_NORMAL);
        gBufferAlbedo = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_ALBEDO);
        gBufferAmbient = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_AMBIENT);
        gBufferSpecular = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_SPECULAR);
        gBufferDepth = builder.getSharedData().get<FrameGraphResource>(GBufferPass::GEOMETRY_BUFFER_DEPTH);
    }

    void execute(FrameGraphPassResources &resources) override {

    }

    std::type_index getTypeIndex() override {
        return typeid(TestPass);
    }

private:
    FrameGraphResource gBufferPosition;
    FrameGraphResource gBufferNormal;
    FrameGraphResource gBufferAlbedo;
    FrameGraphResource gBufferAmbient;
    FrameGraphResource gBufferSpecular;
    FrameGraphResource gBufferDepth;
};

#endif //XENGINE_TESTPASS_HPP
