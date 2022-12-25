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

#include "xng/xng.hpp"

#include <fstream>
#include <iostream>
#include <filesystem>

void printUsage() {
    std::cout << "Usage: PROGRAM PathToScene";
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printUsage();
        return 0;
    }

    std::filesystem::path filePath = std::filesystem::path(argv[1]);

    std::fstream fs(filePath);

    if (!fs.is_open()) {
        std::cout << "Failed to open file " + filePath.string() << "\n";
        return 0;
    }

    xng::JsonParser parser;
    auto bundle = parser.read(xng::readFile(filePath), filePath.extension(), nullptr);

    for (auto &pair: bundle.assets) {
        if (pair.second->getTypeIndex() == typeid(xng::EntityScene)) {
            std::cout << "EntityScene: " + dynamic_cast<xng::EntityScene*>(pair.second.get())->getName() << "\n";
        }
    }

    return 0;
}