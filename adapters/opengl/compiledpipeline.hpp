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

#ifndef XENGINE_COMPILEDPIPELINE_HPP
#define XENGINE_COMPILEDPIPELINE_HPP

#include "xng/rendergraph/shader/shader.hpp"

using namespace xng;

struct CompiledPipeline {
    std::unordered_map<Shader::Stage, std::string> sourceCode;

    std::vector<std::string> shaderBufferBindings;
    std::vector<std::string> textureArrayBindings;

    std::unordered_map<std::string, size_t> textureArraySizes;

    size_t getShaderBufferBinding(const std::string &name) const {
        for (auto i = 0; i < shaderBufferBindings.size(); ++i) {
            if (shaderBufferBindings.at(i) == name) {
                return i;
            }
        }
        throw std::runtime_error("Shader buffer " + name + " not found");
    }

    size_t getTextureArrayBinding(const std::string &name) const {
        size_t ret = 0;
        for (auto &binding : textureArrayBindings) {
            if (binding == name) {
                return ret;
            }
            ret += textureArraySizes.at(binding);
        }
        throw std::runtime_error("Texture Array " + name + " not found");
    }

    size_t createShaderBufferBinding(const std::string &name) {
        for (auto i = 0; i < shaderBufferBindings.size(); ++i) {
            auto &binding = shaderBufferBindings[i];
            if (name == binding) {
                return i;
            }
        }
        shaderBufferBindings.emplace_back(name);
        return shaderBufferBindings.size() - 1;
    }

    size_t createTextureArrayBinding(const std::string &name, size_t arraySize) {
        bool found = false;
        size_t ret = 0;
        for (auto &binding : textureArrayBindings) {
            if (name == binding) {
                found = true;
                break;
            }
            ret += textureArraySizes.at(binding);
        }
        if (!found) {
            textureArrayBindings.emplace_back(name);
            textureArraySizes[name] = arraySize;
        }
        return ret;
    }
};

#endif //XENGINE_COMPILEDPIPELINE_HPP