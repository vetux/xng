/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_LIBRARYWIN32_HPP
#define XENGINE_LIBRARYWIN32_HPP

#include "xng/io/library.hpp"

#include <string>
#include <windows.h>

namespace xng {
    class LibraryWin32 : public Library {
    public:
        explicit LibraryWin32(const std::string &filePath)
                : filePath(filePath){
            handle = LoadLibrary(TEXT(filePath.c_str()));
            if (handle == nullptr){
                throw std::runtime_error("Failed to open library at: " + filePath);
            }
        }

        ~LibraryWin32() override {
            FreeLibrary(handle);
        }

        void *getSymbolAddress(const std::string &address) override {
            return GetProcAddress(handle, address.c_str());
        }

        std::string filePath;
        HINSTANCE handle;
    };

    typedef LibraryWin32 LibraryOS;
}

#endif //XENGINE_LIBRARYWIN32_HPP
