/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_GLSLANG_HPP
#define XENGINE_GLSLANG_HPP

#include "shader/shadercompiler.hpp"

namespace xng::glslang {
    class XENGINE_EXPORT GLSLang : public ShaderCompiler {
    public:
        GLSLang();

        ~GLSLang();

        std::vector<uint32_t> compile(const std::string &source,
                                      const std::string &entryPoint,
                                      ShaderStage stage,
                                      ShaderLanguage language,
                                      OptimizationLevel optimizationLevel,
                                      ShaderEnvironment environment) const override;

        std::string preprocess(const std::string &source,
                               ShaderStage stage,
                               ShaderLanguage language,
                               const std::function<std::string(const char *)> &include,
                               const std::map<std::string, std::string> &macros,
                               OptimizationLevel optimizationLevel,
                               ShaderEnvironment environment) const override;
    };
}

#endif //XENGINE_GLSLANG_HPP
