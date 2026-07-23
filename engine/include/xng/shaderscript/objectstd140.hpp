/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_OBJECTSTD140_HPP
#define XENGINE_OBJECTSTD140_HPP

#include <cassert>
#include <cstring>
#include <vector>

#include "xng/shaderscript/layoutstd140.hpp"

namespace xng {
    class ObjectStd140 {
    public:
        explicit ObjectStd140(const LayoutStd140 &layout)
            : layout(layout), data(layout.getTotalSize()) {
        }

        void set(const std::string &name, const rg::ShaderPrimitive &primitive) {
            const auto primitiveType = primitive.getType();
            switch (primitiveType.type) {
                case rg::ShaderPrimitiveType::SCALAR:
                    switch (primitiveType.component) {
                        case rg::ShaderPrimitiveType::BOOLEAN:
                            set<bool>(name, std::get<bool>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::UNSIGNED_INT:
                            set<unsigned int>(name, std::get<unsigned int>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::SIGNED_INT:
                            set<int>(name, std::get<int>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::FLOAT:
                            set<float>(name, std::get<float>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            set<double>(name, std::get<double>(primitive.value));
                            break;
                    }
                    break;
                case rg::ShaderPrimitiveType::VECTOR2:
                    switch (primitiveType.component) {
                        case rg::ShaderPrimitiveType::BOOLEAN:
                            set<Vec2b>(name, std::get<Vec2b>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::UNSIGNED_INT:
                            set<Vec2u>(name, std::get<Vec2u>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::SIGNED_INT:
                            set<Vec2i>(name, std::get<Vec2i>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::FLOAT:
                            set<Vec2f>(name, std::get<Vec2f>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            set<Vec2d>(name, std::get<Vec2d>(primitive.value));
                            break;
                    }
                    break;
                case rg::ShaderPrimitiveType::VECTOR3:
                    switch (primitiveType.component) {
                        case rg::ShaderPrimitiveType::BOOLEAN:
                            set<Vec3b>(name, std::get<Vec3b>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::UNSIGNED_INT:
                            set<Vec3u>(name, std::get<Vec3u>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::SIGNED_INT:
                            set<Vec3i>(name, std::get<Vec3i>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::FLOAT:
                            set<Vec3f>(name, std::get<Vec3f>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            set<Vec3d>(name, std::get<Vec3d>(primitive.value));
                            break;
                    }
                    break;
                case rg::ShaderPrimitiveType::VECTOR4:
                    switch (primitiveType.component) {
                        case rg::ShaderPrimitiveType::BOOLEAN:
                            set<Vec4b>(name, std::get<Vec4b>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::UNSIGNED_INT:
                            set<Vec4u>(name, std::get<Vec4u>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::SIGNED_INT:
                            set<Vec4i>(name, std::get<Vec4i>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::FLOAT:
                            set<Vec4f>(name, std::get<Vec4f>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            set<Vec4d>(name, std::get<Vec4d>(primitive.value));
                            break;
                    }
                    break;
                case rg::ShaderPrimitiveType::MAT2:
                    switch (primitiveType.component) {
                        case rg::ShaderPrimitiveType::FLOAT:
                            set<Mat2f>(name, std::get<Mat2f>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            set<Mat2d>(name, std::get<Mat2d>(primitive.value));
                            break;
                        default:
                            throw std::runtime_error("Unsupported matrix type");
                    }
                    break;
                case rg::ShaderPrimitiveType::MAT3:
                    switch (primitiveType.component) {
                        case rg::ShaderPrimitiveType::FLOAT:
                            set<Mat3f>(name, std::get<Mat3f>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            set<Mat3d>(name, std::get<Mat3d>(primitive.value));
                            break;
                        default:
                            throw std::runtime_error("Unsupported matrix type");
                    }
                    break;
                case rg::ShaderPrimitiveType::MAT4:
                    switch (primitiveType.component) {
                        case rg::ShaderPrimitiveType::FLOAT:
                            set<Mat4f>(name, std::get<Mat4f>(primitive.value));
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            set<Mat4d>(name, std::get<Mat4d>(primitive.value));
                            break;
                        default:
                            throw std::runtime_error("Unsupported matrix type");
                    }
                    break;
                default:
                    throw std::runtime_error("Unsupported primitive type");
            }
        }

        template<typename T>
        void set(const std::string &name, const T &value) {
            assert(std::holds_alternative<rg::ShaderPrimitiveType>(layout.getStructType().get(name).type.value));
            if (layout.getStructType().get(name).type.getPrimitive() != rg::ShaderPrimitive(value).getType()) {
                throw std::runtime_error("Invalid member type for " + name);
            }

            Std140<T> valueAligned(value);

            const auto offset = layout.getOffset(name);

            assert(offset + sizeof(Std140<T>) <= data.size());

            std::memcpy(data.data() + offset, &valueAligned, Std140Size<T>::value);
        }

        const std::vector<uint8_t> &getData() const { return data; }

    private:
        LayoutStd140 layout;
        std::vector<uint8_t> data;
    };
}

#endif //XENGINE_OBJECTSTD140_HPP
