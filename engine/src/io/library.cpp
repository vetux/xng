/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/io/library.hpp"

#ifdef __linux__
#include "io/dl/librarylinux.hpp"
#else
#ifdef _WIN32
#include "io/dl/librarywin32.hpp"
#endif
#endif

namespace xng {
    std::unique_ptr <Library> Library::load(const std::string &path) {
        return std::make_unique<LibraryOS>(path);
    }
}

const char *xng::Library::getPlatformFilePrefix() {
#ifdef __linux__
return "lib";
#else
#ifdef _WIN32
return "";
#endif
#endif
}

const char *xng::Library::getPlatformFileExtension() {
#ifdef __linux__
return ".so";
#else
#ifdef _WIN32
return ".dll";
#endif
#endif
}
