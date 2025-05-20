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

#include "xng/driver/opengl/fgruntimeogl.hpp"

void xng::FGRuntimeOGL::setWindow(const xng::Window &window) {

}

/**
 * Generate shader source code
 * Allocate vertex / index buffer and upload data
 *
 * @param graph
 * @return
 */
xng::FGRuntime::GraphHandle xng::FGRuntimeOGL::compile(const xng::FGGraph &graph) {
    return 0;
}

void xng::FGRuntimeOGL::execute(xng::FGRuntime::GraphHandle graph) {

}

void xng::FGRuntimeOGL::saveCache(xng::FGRuntime::GraphHandle graph, std::ostream &stream) {

}

void xng::FGRuntimeOGL::loadCache(xng::FGRuntime::GraphHandle graph, std::istream &stream) {

}
