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

#ifndef XENGINE_SHADER_INCLUDER_HPP
#define XENGINE_SHADER_INCLUDER_HPP

#pragma message "Not Implemented"
/*
#include "xng/render/shader/shaderinclude.hpp"
#include "xng/render/shader/include/glsl_mvp.hpp"
#include "xng/render/shader/include/glsl_phong_shading.hpp"
#include "xng/render/shader/include/glsl_pi.hpp"
#include "xng/render/shader/include/glsl_noise.hpp"
#include "xng/render/shader/include/glsl_texfilter.hpp"

static std::string includeCallback(const char *n) {
    std::string name(n);
    if (name == "phong.glsl") {
        return GLSL_PHONG_SHADING;
    } else if (name == "mvp.glsl") {
        return GLSL_MVP;
    } else if (name == "noise.glsl") {
        return GLSL_NOISE;
    } else if (name == "pi.glsl") {
        return GLSL_PI;
    } else if (name == "texfilter.glsl") {
        return GLSL_TEXFILTER;
    } else {
        return "";
    }
}

static const std::map<std::string, std::string> gMacros = {{"MAX_LIGHTS", "20"}};

static const std::function<std::string(const char *)> gIncludeFunc = {includeCallback};

namespace xng {
    const std::map<std::string, std::string> &ShaderInclude::getShaderMacros(ShaderLanguage lang) {
        switch (lang) {
            case HLSL_SHADER_MODEL_4:
            case GLSL_410:
            case GLSL_ES_320:
                return gMacros;
            default:
                throw std::runtime_error("Not implemented");
        }
    }

    const std::function<std::string(const char *)> &ShaderInclude::getShaderIncludeCallback() {
        return gIncludeFunc;
    }
}
 */
#endif //XENGINE_SHADER_INCLUDER_HPP
