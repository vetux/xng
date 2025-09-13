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

#include "xng/render/shaderlib/noise.hpp"

#include "xng/rendergraph/shaderscript/shaderscript.hpp"

//TODO: Port noise.glsl

using namespace xng::ShaderScript;
DEFINE_FUNCTION1(permute)
DEFINE_FUNCTION1(taylorInvSqrt)

namespace xng::shaderlib {
    void loadPermute() {
        for (auto &func: ShaderBuilder::instance().getFunctions()) {
            if (func.name == "permute") {
                return;
            }
        }

        Function("permute", {{"x", ShaderDataType::float32()}}, ShaderDataType::float32());
        {
            Float x = argument("x");
            Return(floor(mod(((x * 34.0) + 1.0) * x, 289.0)));
        }
        EndFunction();

        Function("permute", {{"x", ShaderDataType::vec3()}}, ShaderDataType::vec3());
        {
            vec3 x = argument("x");
            Return(mod(((x * 34.0) + 1.0) * x, 289.0));
        }
        EndFunction();

        Function("permute", {{"x", ShaderDataType::vec4()}}, ShaderDataType::vec4());
        {
            vec4 x = argument("x");
            Return(mod(((x * 34.0) + 1.0) * x, 289.0));
        }
        EndFunction();
    }

    void loadTaylorInvSqrt() {
        for (auto &func: ShaderBuilder::instance().getFunctions()) {
            if (func.name == "taylorInvSqrt") {
                return;
            }
        }
        Function("taylorInvSqrt", {{"r", ShaderDataType::float32()}}, ShaderDataType::float32());
        {
            Float r = argument("r");
            Return(Float(1.79284291400159) - Float(0.85373472095314) * r);
        }
        EndFunction();

        Function("taylorInvSqrt", {{"r", ShaderDataType::vec4()}}, ShaderDataType::vec4());
        {
            vec4 r = argument("r");
            Return(Float(1.79284291400159) - Float(0.85373472095314) * r);
        }
        EndFunction();
    }

    void noise::simplex() {
        for (auto &func: ShaderBuilder::instance().getFunctions()) {
            if (func.name == "simplex") {
                return;
            }
        }

        loadPermute();
        loadTaylorInvSqrt();
        Function("simplex", {{"v", ShaderDataType::vec2()}}, ShaderDataType::float32());
        {
            vec2 v = argument("v");
            vec4 C = vec4(0.211324865405187, 0.366025403784439,
                          -0.577350269189626, 0.024390243902439);
            vec2 i = floor(v + dot(v, vec2(C.y(), C.y())));
            vec2 x0 = v - i + dot(i, vec2(C.x(), C.x()));

            vec2 i1;
            If(x0.x() > x0.y());
            {
                i1 = vec2(1.0, 0.0);
            }
            Else();
            {
                i1 = vec2(0.0, 1.0);
            }
            EndIf();

            vec4 x12 = vec4(x0.x(), x0.y(), i1.x(), i1.y())
                       + vec4(C.x(), C.x(), C.z(), C.z());
            x12.setX(x12.x() - i1.x());
            x12.setY(x12.y() - i1.y());

            i = mod(i, 289.0);
            vec3 p = permute(permute(i.y() + vec3(0.0, i1.y(), 1.0))
                             + i.x()
                             + vec3(0.0, i1.x(), 1.0));
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
        }
        EndFunction();
    }

    void noise::perlin() {
        throw std::runtime_error("Perlin not implemented");
    }

    void noise::noise() {
        throw std::runtime_error("Noise not implemented");
    }
}
