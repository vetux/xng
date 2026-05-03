/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
