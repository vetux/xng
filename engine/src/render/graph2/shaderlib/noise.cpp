/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "xng/render/graph2/shaderlib/noise.hpp"

#include "xng/render/graph2/shader/fgshaderfactory.hpp"

#include "xng/render/graph2/shader/fgshadernodehelper.hpp"

//TODO: Port noise.glsl to noise.cpp

using namespace xng::FGShaderNodeHelper;

namespace xng::shaderlib {
    FGShaderNodeWrapper permute(const FGShaderNodeWrapper &x) {
        return floor(mod(((x * 34.0) + 1.0) * x, 289.0));
    }

    FGShaderNodeWrapper taylorInvSqrt(const FGShaderNodeWrapper &r) {
        return Float(1.79284291400159) - Float(0.85373472095314) * r;
    }

    FGShaderFunction noise::simplex2D() {
        auto &builder = FGShaderBuilder::instance();
        builder.setup({}, {}, {}, {}, {}, {}, {});

        builder.Function("simplex2D", {{"v", FGShaderValue::vec2()}}, FGShaderValue::float32());

        vec2 v = argument("v");
        const vec4 C = vec4(0.211324865405187, 0.366025403784439,
                            -0.577350269189626, 0.024390243902439);
        vec2 i = floor(v + dot(v, vec2(C.y(), C.y())));
        vec2 x0 = v - i + dot(i, vec2(C.x(), C.x()));

        vec2 i1 = vec2(0, 0);
        builder.If(x0.x() > x0.y());
        i1 = vec2(1.0, 0.0);
        builder.Else();
        i1 = vec2(0.0, 1.0);
        builder.EndIf();

        vec4 x12 = vec4(x0.x(), x0.y(), i1.x(), i1.y())
                   + vec4(C.x(), C.x(), C.z(), C.z());
        x12.setX(x12.x() - i1.x());
        x12.setY(x12.y() - i1.y());

        i = mod(i, 289.0);
        vec3 p = permute(permute(i.y() + vec3(0.0, i1.y(), 1.0))
                         + i.x() + vec3(0.0, i1.x(), 1.0));
        vec3 m = max(0.5 - vec3(dot(x0, x0), dot(vec2(x12.x(), x12.y()), vec2(x12.x(), x12.y())),
                                dot(vec2(x12.z(), x12.w()), vec2(x12.z(), x12.w()))), 0.0);
        m = m * m;
        m = m * m;
        vec3 x = Float(2.0) * fract(p * vec3(C.w(), C.w(), C.w())) - 1.0;
        vec3 h = abs(x) - 0.5;
        vec3 ox = floor(x + 0.5);
        vec3 a0 = x - ox;
        m *= Float(1.79284291400159) - Float(0.85373472095314) * (a0 * a0 + h * h);
        vec3 g = vec3(0, 0, 0);
        g.setX(a0.x() * x0.x() + h.x() * x0.y());
        vec2 g_ = vec2(a0.y(), a0.z())
                  * vec2(x12.x(), x12.z())
                  * vec2(h.y(), h.z())
                  * vec2(x12.y(), x12.w());
        g.setY(g_.x());
        g.setZ(g_.y());
        Return(Float(130.0) * dot(m, g));

        builder.EndFunction();

        return builder.getFunctions().at("simplex2D");
    }

    FGShaderFunction noise::simplex3D() {
        throw std::runtime_error("Simplex3D not implemented");
    }

    FGShaderFunction noise::simplex4D() {
        throw std::runtime_error("Simplex4D not implemented");
    }

    FGShaderFunction noise::perlin() {
        throw std::runtime_error("Perlin not implemented");
    }

    FGShaderFunction noise::perlinT() {
        throw std::runtime_error("PerlinT not implemented");
    }

    FGShaderFunction noise::perlinBW() {
        throw std::runtime_error("PerlinBW not implemented");
    }

    FGShaderFunction noise::perlinC2D() {
        throw std::runtime_error("PerlinC2D not implemented");
    }

    FGShaderFunction noise::perlinC3D() {
        throw std::runtime_error("PerlinC3D not implemented");
    }

    FGShaderFunction noise::perlinPeriodic() {
        throw std::runtime_error("PerlinPeriodic not implemented");
    }

    FGShaderFunction noise::noise() {
        throw std::runtime_error("Noise not implemented");
    }

    FGShaderFunction noise::noise2D() {
        throw std::runtime_error("Noise2D not implemented");
    }

    FGShaderFunction noise::noise3D() {
        throw std::runtime_error("Noise3D not implemented");
    }
}
