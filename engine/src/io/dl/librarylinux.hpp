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

#ifndef XENGINE_LIBRARYLINUX_HPP
#define XENGINE_LIBRARYLINUX_HPP

#include "xng/io/library.hpp"

#include <string>
#include <dlfcn.h>

namespace xng {
    class LibraryLinux : public Library {
    public:
        explicit LibraryLinux(const std::string &filePath)
                : filePath(filePath) {
            handle = dlopen(filePath.c_str(), RTLD_LAZY);
            if (handle == nullptr){
                throw std::runtime_error("Failed to open library at: " + filePath + " Error: " + dlerror());
            }
        }

        ~LibraryLinux() override {
            dlclose(handle);
        }

        void *getSymbolAddress(const std::string &address) override {
            return dlsym(handle, address.c_str());
        }

        std::string filePath;
        void *handle;
    };

    typedef LibraryLinux LibraryOS;
}

#endif //XENGINE_LIBRARYLINUX_HPP
