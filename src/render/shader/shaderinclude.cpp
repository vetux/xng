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

#ifndef MANA_SHADER_INCLUDER_HPP
#define MANA_SHADER_INCLUDER_HPP

#include "render/shader/shaderinclude.hpp"
#include "render/shader/include/glsl_mvp.hpp"
#include "render/shader/include/hlsl_mvp.hpp"
#include "render/shader/include/glsl_phong_shading.hpp"
#include "render/shader/include/hlsl_phong_shading.hpp"
#include "render/shader/include/glsl_pi.hpp"
#include "render/shader/include/hlsl_pi.hpp"
#include "render/shader/include/glsl_noise.hpp"
#include "render/shader/include/hlsl_noise.hpp"

static std::string includeCallback(const char *n) {
    std::string name(n);
    if (name == "phong.hlsl") {
        return HLSL_PHONG_SHADING;
    } else if (name == "phong.glsl") {
        return GLSL_PHONG_SHADING;
    } else if (name == "mvp.hlsl") {
        return HLSL_MVP;
    } else if (name == "mvp.glsl") {
        return GLSL_MVP;
    } else if (name == "noise.hlsl") {
        return HLSL_NOISE;
    } else if (name == "noise.glsl") {
        return GLSL_NOISE;
    } else if (name == "pi.glsl") {
        return HLSL_PI;
    } else if (name == "pi.glsl") {
        return GLSL_PI;
    } else {
        throw std::runtime_error("Invalid name: " + name);
    }
}

static const std::map<std::string, std::string> gMacros = {{"MAX_LIGHTS", "20"}};

static const std::function<std::string(const char *)> gIncludeFunc = {includeCallback};

namespace xengine {
    const std::map<std::string, std::string> &ShaderInclude::getShaderMacros(ShaderLanguage lang) {
        switch (lang) {
            case HLSL_SHADER_MODEL_4:
            case GLSL_460:
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
#endif //MANA_SHADER_INCLUDER_HPP
