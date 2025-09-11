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

//TODO: Port noise.glsl to noise.cpp

using namespace xng::FGShaderFactory;

namespace xng::shaderlib {
    std::unique_ptr<FGShaderNode> permute(const std::unique_ptr<FGShaderNode> &x) {
        return floor(
            mod(
                add(
                    multiply(x, literal(34.0f)),
                    literal(1.0f)),
                literal(289.0f)
            ));
    }

    FGShaderFunction noise::simplex2D() {
        throw std::runtime_error("Simplex2D not implemented");
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
