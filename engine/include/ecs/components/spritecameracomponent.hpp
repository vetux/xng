/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_SPRITECAMERACOMPONENT_HPP
#define XENGINE_SPRITECAMERACOMPONENT_HPP

namespace xng {
    /**
     * A sprite camera is a orthographic camera with the bounds set to the screen dimensions and 0,0 at the top left of the screen
     * It is used by the sprite render system when rendering sprites.
     * It can be offset in the x and y and rotated in the z using the transform component values.
     */
    struct SpriteCameraComponent {
        float nearClip = 0.1f;
        float farClip = 1000.0f;
    };
}

#endif //XENGINE_SPRITECAMERACOMPONENT_HPP
