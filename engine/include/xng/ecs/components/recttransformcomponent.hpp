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

#ifndef XENGINE_RECTTRANSFORMCOMPONENT_HPP
#define XENGINE_RECTTRANSFORMCOMPONENT_HPP

#include "xng/ecs/component.hpp"

#include "xng/gui/recttransform.hpp"

namespace xng {
    /**
     * Describe a rectangular area on a canvas.
     *
     * The entity must contain a TransformComponent.
     * The parent of the TransformComponent must be the name of an entity which contains a RectTransformComponent.
     * Alignments and positions are relative to the specified parent.
     * The top level parent of RectTransformComponents must contain a CanvasComponent.
     * If the entity contains a TransformComponent the x/y position divided by CanvasRenderSystem.pixelToMeter and z rotation of the transform are added
     * to the rect transform values and the TransformComponent x/y scale is used to scale the rect size when rendering
     * which allows a RectTransformComponent to be animated by RigidbodyComponents.
     */
    struct XENGINE_EXPORT RectTransformComponent : public Component {
        RectTransform rectTransform;

        Messageable &operator<<(const Message &message) override {
            message.value("rectTransform", rectTransform);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            rectTransform >> message["rectTransform"];
            return message;
        }

        std::type_index getType() const override {
            return typeid(RectTransformComponent);
        }
    };
}

#endif //XENGINE_RECTTRANSFORMCOMPONENT_HPP
