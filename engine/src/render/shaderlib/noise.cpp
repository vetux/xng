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
            Return(floor(mod(((x * 34.0f) + 1.0f) * x, 289.0f)));
        }
        EndFunction();

        Function("permute", {{"x", ShaderDataType::vec3()}}, ShaderDataType::vec3());
        {
            vec3 x = argument("x");
            Return(mod(((x * 34.0f) + 1.0f) * x, 289.0f));
        }
        EndFunction();

        Function("permute", {{"x", ShaderDataType::vec4()}}, ShaderDataType::vec4());
        {
            vec4 x = argument("x");
            Return(mod(((x * 34.0f) + 1.0f) * x, 289.0f));
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
            Return(Float(1.79284291400159f) - Float(0.85373472095314f) * r);
        }
        EndFunction();

        Function("taylorInvSqrt", {{"r", ShaderDataType::vec4()}}, ShaderDataType::vec4());
        {
            vec4 r = argument("r");
            Return(Float(1.79284291400159f) - Float(0.85373472095314f) * r);
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
            vec4 C = vec4(0.211324865405187f, 0.366025403784439f,
                          -0.577350269189626f, 0.024390243902439f);
            vec2 i = floor(v + dot(v, C.yy()));
            vec2 x0 = v - i + dot(i, C.xx());

            vec2 i1;
            If(x0.x() > x0.y());
            {
                i1 = vec2(1.0, 0.0);
            }
            Else();
            {
                i1 = vec2(0.0f, 1.0f);
            }
            EndIf();

            vec4 x12 = x0.xyxy() + C.xxzz();
            x12.xy() -= i1;

            i = mod(i, 289.0f);
            vec3 p = permute(permute(i.y() + vec3(0.0f, i1.y(), 1.0f))
                             + i.x()
                             + vec3(0.0f, i1.x(), 1.0f));
            vec3 m = max(0.5f - vec3(dot(x0, x0), dot(x12.xy(), x12.xy()),
                                    dot(x12.zw(), x12.zw())), 0.0f);
            m = m * m;
            m = m * m;
            vec3 x = Float(2.0f) * fract(p * vec3(C.w(), C.w(), C.w())) - 1.0;
            vec3 h = abs(x) - 0.5f;
            vec3 ox = floor(x + 0.5f);
            vec3 a0 = x - ox;
            m *= Float(1.79284291400159f) - Float(0.85373472095314f) * (a0 * a0 + h * h);
            vec3 g = vec3(0, 0, 0);
            g.x() = a0.x() * x0.x() + h.x() * x0.y();
            g.yz() = a0.yz() * x12.xz() + h.yz() * x12.yw();
            Return(Float(130.0f) * dot(m, g));
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
