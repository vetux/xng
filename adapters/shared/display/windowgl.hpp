/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_WINDOWGL_HPP
#define XENGINE_WINDOWGL_HPP

namespace xng {
    /**
     * The glfw display adapter implements this interface for OPENGL_4_6 windows.
     *
     * The opengl adapter depends on this interface.
     * The window passed to opengl::Runtime.createSurface must implement this interface.
     */
    class WindowGl {
    public:
        virtual ~WindowGl() = default;

        /**
         * Make the window-owned OpenGL context current.
         */
        virtual void makeContextCurrent() = 0;

        /**
         * Swap the window buffers.
         */
        virtual void swapBuffers() = 0;
    };
}

#endif //XENGINE_WINDOWGL_HPP