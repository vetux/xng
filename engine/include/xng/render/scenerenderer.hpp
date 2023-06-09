/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_SCENERENDERER_HPP
#define XENGINE_SCENERENDERER_HPP

#include "scene.hpp"

#include "xng/types/genericmap.hpp"

#include "xng/gpu/rendertarget.hpp"

namespace xng {
    /**
     * A scene renderer produces an image from a given scene using the set properties.
     *
     * Properties are renderer specific.
     *
     * To integrate 3rd party renderers or create a renderer which doesnt use the gpu / display abstraction
     * the user can either subclass scene renderer and pass the instance to the mesh render system
     * or subclass system to create a custom rendering system.
     */
    class XENGINE_EXPORT SceneRenderer {
    public:
        virtual void render(const Scene &scene) = 0;

        virtual void setProperties(const GenericMapString &value) = 0;

        virtual GenericMapString &getProperties() = 0;

        virtual const GenericMapString &getProperties() const = 0;
    };
}
#endif //XENGINE_SCENERENDERER_HPP
