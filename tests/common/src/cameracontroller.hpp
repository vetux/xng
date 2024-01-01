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

#ifndef XENGINE_CAMERACONTROLLER_HPP
#define XENGINE_CAMERACONTROLLER_HPP

#include "xng/xng.hpp"

using namespace xng;

class CameraController {
public:
    float rotationSpeed = 5;
    float movementSpeed = 10;

    CameraController(Transform &transform, Input &input)
            : transform(transform), input(input) {}

    void update(DeltaTime delta) {
        if (input.getMouse().getButtonDown(xng::MOUSE_BUTTON_RIGHT)) {
            lockCursor = true;
            input.setMouseCursorMode(xng::Input::CURSOR_DISABLED);
        } else if (!input.getMouse().getButton(MOUSE_BUTTON_RIGHT)) {
            lockCursor = false;
            input.setMouseCursorMode(xng::Input::CURSOR_NORMAL);
        }

        Vec2f rotation;

        if (lockCursor) {
            rotation = Vec2f(-input.getMouse().positionDelta.y, -input.getMouse().positionDelta.x);
        }

        if (input.getKey(KEY_Q)){
            rotation.y += -5;
        } else if (input.getKey(xng::KEY_E)){
            rotation.y += 5;
        }

        float speed = 1;
        if (input.getKey(KEY_LSHIFT)){
            speed = 10;
        }

        if (rotation.magnitude() > 0){
            transform.applyRotation(Quaternion(Vec3f(rotation.x * rotationSpeed * delta, 0, 0)),
                                    false);
            transform.applyRotation(Quaternion(Vec3f(0, rotation.y * rotationSpeed * delta, 0)),
                                    true);
        }

        Vec3f movement;
        if (input.getKey(KEY_W)) {
            movement.y = -speed;
        } else if (input.getKey(xng::KEY_S)) {
            movement.y = speed;
        }

        if (input.getKey(KEY_A)) {
            movement.x = speed;
        } else if (input.getKey(xng::KEY_D)) {
            movement.x = -speed;
        }

        if (input.getKey(xng::KEY_SPACE)) {
            movement.z = speed;
        } else if (input.getKey(xng::KEY_LCTRL)) {
            movement.z = -speed;
        }

        if (movement.magnitude() > 0) {
            Vec3f pos = transform.getPosition() + (transform.forward() * movement.y * movementSpeed * delta);
            pos += transform.left() * movement.x * movementSpeed * delta;
            pos += Vec3f(0, movement.z * movementSpeed * delta, 0);
            transform.setPosition(pos);
        }
    }

private:
    Transform &transform;
    Input &input;

    bool lockCursor = false;
};

#endif //XENGINE_CAMERACONTROLLER_HPP
