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

#ifndef XENGINE_DEBUGPASS_HPP
#define XENGINE_DEBUGPASS_HPP

#include "render/deferred/renderpass.hpp"

namespace xengine {
    class XENGINE_EXPORT DebugPass : public RenderPass {
    public:
        explicit DebugPass(RenderDevice &device);

        ~DebugPass() override;

        void render(GBuffer &gBuffer, Scene &scene) override;

        void setEnabled(bool enable) { enabled = enable; }

    private:
        RenderDevice &device;

        ShaderSource vs;
        ShaderSource gs;
        ShaderSource fs;

        ShaderSource vsl;
        ShaderSource gsl;

        ShaderSource vsw;
        ShaderSource gsw;

        bool enabled = false;

        std::unique_ptr<ShaderProgram> shaderWireframe;
        std::unique_ptr<ShaderProgram> shaderNormals;
        std::unique_ptr<ShaderProgram> shaderLight;
        std::unique_ptr<MeshBuffer> meshBuffer;
        std::unique_ptr<RenderTarget> multiSampleTarget;
    };
}

#endif //XENGINE_DEBUGPASS_HPP
