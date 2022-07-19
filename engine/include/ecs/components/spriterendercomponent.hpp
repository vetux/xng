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

#ifndef XENGINE_SPRITECOMPONENT_HPP
#define XENGINE_SPRITECOMPONENT_HPP

#include "asset/sprite.hpp"

namespace xng {
    /**
     * Sprite components are rendered by a sprite render system independently of the mesh render system.
     *
     * The transform component euler rotation z component is used for rotating the sprites around the transform center,
     * and the transform position x and y component is used for positioning the sprite and the z component is used for layering.
     */
    struct XENGINE_EXPORT SpriteRenderComponent {
        bool enabled;
        Rectf displayRect{}; // The offset of the transform from the top left of the sprite and the display size in pixels
        ResourceHandle<Sprite> sprite; // The sprite to draw
        int layer; // The drawing layer of the sprite
    };
}

#endif //XENGINE_SPRITECOMPONENT_HPP
