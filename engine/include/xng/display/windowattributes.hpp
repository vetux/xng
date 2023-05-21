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

#ifndef XENGINE_WINDOWATTRIBUTES_HPP
#define XENGINE_WINDOWATTRIBUTES_HPP

namespace xng {
    /**
     * The attributes of a window which are required for creation.
     * Some attributes may be changed for an existing window, for which
     * individual member functions are added to the window class XENGINE_EXPORT.
     */
    struct WindowAttributes {
        bool resizable = true;
        bool visible = true;
        bool decorated = true;
        bool focused = true;
        bool autoMinimize = true;
        bool alwaysOnTop = false;
        bool maximized = false;
        bool centerCursor = true;
        bool transparentFramebuffer = false;
        bool focusOnShow = true;
        bool scaleToMonitor = false;

        int bitsRed = 8;
        int bitsGreen = 8;
        int bitsBlue = 8;
        int bitsAlpha = 8;
        int bitsDepth = 24;
        int bitsStencil = 8;

        bool multiSample = false;
        int samples = 0;

        bool sRGBCapable = false;

        bool doubleBuffer = true;

        int swapInterval = 0;

        bool debug = false; // Only affects OpenGL display driver backend, If true the window will contain a debug context.
    };
}

#endif //XENGINE_WINDOWATTRIBUTES_HPP
