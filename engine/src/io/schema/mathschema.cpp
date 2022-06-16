/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "io/schema/mathschema.hpp"

namespace xengine {
    Grid &operator<<(Grid &value, const Message &message) {
        value.size << message["size"];
        value.nCol = message["nCol"];
        value.nRow = message["nRow"];
        return value;
    }

    Message &operator<<(Message &message, const Grid &value) {
        auto map = std::map<std::string, Message>();
        map["size"] << value.size;
        map["nCol"] = value.nCol;
        map["nRow"] = value.nRow;
        message = map;
        return message;
    }

    //TODO: Implement matrix schema
    Mat4f &operator<<(Mat4f &matrix, const Message &message) {
        throw std::runtime_error("Not implemented");
    }

    Message &operator<<(Message &message, const Mat4f &value) {
        throw std::runtime_error("Not implemented");
    }

    Rectf &operator<<(Rectf &mat, const Message &message) {
        mat.position << message["position"];
        mat.dimensions << message["dimensions"];
        return mat;
    }

    Message &operator<<(Message &message, const Rectf &value) {
        auto map = std::map<std::string, Message>();
        map["position"] << value.position;
        map["dimensions"] << value.dimensions;
        return message;
    }

    Transform &operator<<(Transform &value, const Message &message) {
        Vec3f tmp;
        tmp << message["position"];
        value.setPosition(tmp);
        tmp << message["rotation"];
        value.setRotation(Quaternion(tmp));
        tmp << message["scale"];
        value.setScale(tmp);
        return value;
    }

    Message &operator<<(Message &message, const Transform &value) {
        auto map = std::map<std::string, Message>();
        map["position"] << value.getPosition();
        map["rotation"] << value.getRotation().getEulerAngles();
        map["scale"] << value.getScale();
        message = map;
        return message;
    }

    Vec2f &operator<<(Vec2f &value, const Message &message) {
        value.x = message["x"];
        value.y = message["y"];
        return value;
    }

    Message &operator<<(Message &message, const Vec2f &value) {
        auto map = std::map<std::string, Message>();
        map["x"] = value.x;
        map["y"] = value.y;
        message = map;
        return message;
    }

    Vec2i &operator<<(Vec2i &value, const Message &message) {
        value.x = message["x"];
        value.y = message["y"];
        return value;
    }

    Message &operator<<(Message &message, const Vec2i &value) {
        auto map = std::map<std::string, Message>();
        map["x"] = value.x;
        map["y"] = value.y;
        message = map;
        return message;
    }

    Vec3f &operator<<(Vec3f &value, const Message &message) {
        value.x = message.value<float>("x", message.value<float>("r", 0));
        value.y = message.value<float>("y", message.value<float>("g", 0));
        value.z = message.value<float>("z", message.value<float>("b", 0));
        return value;
    }

    Message &operator<<(Message &message, const Vec3f &value) {
        auto map = std::map<std::string, Message>();
        map["x"] = value.x;
        map["y"] = value.y;
        map["z"] = value.z;
        message = map;
        return message;
    }

    Vec4f &operator<<(Vec4f &value, const Message &message) {
        value.x = message.value<float>("x", message.value<float>("r", 0));
        value.y = message.value<float>("y", message.value<float>("g", 0));
        value.z = message.value<float>("z", message.value<float>("b", 0));
        value.w = message.value<float>("w", message.value<float>("a", 0));
        return value;
    }

    Message &operator<<(Message &message, const Vec4f &value) {
        auto map = std::map<std::string, Message>();
        map["x"] = value.x;
        map["y"] = value.y;
        map["z"] = value.z;
        map["w"] = value.w;
        message = map;
        return message;
    }

    Quaternion &operator<<(Quaternion &q, const Message &message) {
        q.w = message["w"];
        q.x = message["x"];
        q.y = message["y"];
        q.z = message["z"];
        return q;
    }

    Message &operator<<(Message &message, const Quaternion &q) {
        auto map = std::map<std::string, Message>();
        map["w"] = q.w;
        map["x"] = q.x;
        map["y"] = q.y;
        map["z"] = q.z;
        message = map;
        return message;
    }
}