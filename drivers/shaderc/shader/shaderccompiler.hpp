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

#ifndef XENGINE_SHADERCCOMPILER_HPP
#define XENGINE_SHADERCCOMPILER_HPP

#include "xng/shader/spirvcompiler.hpp"

namespace xng {
    class ShaderCCompiler : public SPIRVCompiler {
    public:
        std::type_index getType() override { return typeid(ShaderCCompiler); }

        std::vector<uint32_t> compile(const std::string &source,
                                      const std::string &entryPoint,
                                      ShaderStage stage,
                                      ShaderLanguage language,
                                      OptimizationLevel optimizationLevel) const override;

        std::string preprocess(const std::string &source,
                               ShaderStage stage,
                               ShaderLanguage language,
                               const std::function<std::string(const char *)> &include,
                               const std::map<std::string, std::string> &macros,
                               OptimizationLevel optimizationLevel) const override;
    };
}

#endif //XENGINE_SHADERCCOMPILER_HPP
