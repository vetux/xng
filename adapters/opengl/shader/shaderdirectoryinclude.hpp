/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_SHADERDIRECTORYINCLUDE_HPP
#define XENGINE_SHADERDIRECTORYINCLUDE_HPP

#include <map>
#include <string>
#include <filesystem>
#include <functional>

#include "shader/shaderlanguage.hpp"

namespace xng {
    namespace ShaderDirectoryInclude {
        /**
         * Provide headers from the specified directory.
         * Include paths are parsed relative to the specified path.
         *
         * @param dir
         * @return
         */
        XENGINE_EXPORT std::function<std::string(const char *)> getShaderIncludeCallback(const std::filesystem::path &dir);
    }
}

#endif //XENGINE_SHADERDIRECTORYINCLUDE_HPP
