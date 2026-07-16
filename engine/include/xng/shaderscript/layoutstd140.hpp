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

#ifndef XENGINE_LAYOUTSTD140_HPP
#define XENGINE_LAYOUTSTD140_HPP

#include <string>
#include <unordered_map>
#include <cstdint>

#include "xng/rendergraph/shader/shaderstructtype.hpp"
#include "xng/shaderscript/std140.hpp"

namespace xng {
    class LayoutStd140 {
    public:
        explicit LayoutStd140(const std::string &typeName)
            : structDef(typeName, {}) {
        }

        template<typename T>
        void add(const std::string &name) {
            constexpr size_t alignment = alignof(Std140<T>);
            totalSize = (totalSize + alignment - 1) & ~(alignment - 1);
            offsets[name] = totalSize;
            totalSize += sizeof(Std140<T>);
            structDef.elements.emplace_back(rg::ShaderPrimitive(T()).getType(), name);
        }

        void add(const std::string &name, const rg::ShaderPrimitiveType &primitive) {
            switch (primitive.type) {
                case rg::ShaderPrimitiveType::SCALAR:
                    switch (primitive.component) {
                        case rg::ShaderPrimitiveType::BOOLEAN:
                            add<bool>(name);
                            break;
                        case rg::ShaderPrimitiveType::UNSIGNED_INT:
                            add<unsigned int>(name);
                            break;
                        case rg::ShaderPrimitiveType::SIGNED_INT:
                            add<int>(name);
                            break;
                        case rg::ShaderPrimitiveType::FLOAT:
                            add<float>(name);
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            add<double>(name);
                            break;
                    }
                    break;
                case rg::ShaderPrimitiveType::VECTOR2:
                    switch (primitive.component) {
                        case rg::ShaderPrimitiveType::BOOLEAN:
                            add<Vec2b>(name);
                            break;
                        case rg::ShaderPrimitiveType::UNSIGNED_INT:
                            add<Vec2u>(name);
                            break;
                        case rg::ShaderPrimitiveType::SIGNED_INT:
                            add<Vec2i>(name);
                            break;
                        case rg::ShaderPrimitiveType::FLOAT:
                            add<Vec2f>(name);
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            add<Vec2d>(name);
                            break;
                    }
                    break;
                case rg::ShaderPrimitiveType::VECTOR3:
                    switch (primitive.component) {
                        case rg::ShaderPrimitiveType::BOOLEAN:
                            add<Vec3b>(name);
                            break;
                        case rg::ShaderPrimitiveType::UNSIGNED_INT:
                            add<Vec3u>(name);
                            break;
                        case rg::ShaderPrimitiveType::SIGNED_INT:
                            add<Vec3i>(name);
                            break;
                        case rg::ShaderPrimitiveType::FLOAT:
                            add<Vec3f>(name);
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            add<Vec3d>(name);
                            break;
                    }
                    break;
                case rg::ShaderPrimitiveType::VECTOR4:
                    switch (primitive.component) {
                        case rg::ShaderPrimitiveType::BOOLEAN:
                            add<Vec4b>(name);
                            break;
                        case rg::ShaderPrimitiveType::UNSIGNED_INT:
                            add<Vec4u>(name);
                            break;
                        case rg::ShaderPrimitiveType::SIGNED_INT:
                            add<Vec4i>(name);
                            break;
                        case rg::ShaderPrimitiveType::FLOAT:
                            add<Vec4f>(name);
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            add<Vec4d>(name);
                            break;
                    }
                    break;
                case rg::ShaderPrimitiveType::MAT2:
                    switch (primitive.component) {
                        case rg::ShaderPrimitiveType::FLOAT:
                            add<Mat2f>(name);
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            add<Mat2d>(name);
                            break;
                        default:
                            throw std::runtime_error("Unsupported matrix type");
                    }
                    break;
                case rg::ShaderPrimitiveType::MAT3:
                    switch (primitive.component) {
                        case rg::ShaderPrimitiveType::FLOAT:
                            add<Mat3f>(name);
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            add<Mat3d>(name);
                            break;
                        default:
                            throw std::runtime_error("Unsupported matrix type");
                    }
                    break;
                case rg::ShaderPrimitiveType::MAT4:
                    switch (primitive.component) {
                        case rg::ShaderPrimitiveType::FLOAT:
                            add<Mat4f>(name);
                            break;
                        case rg::ShaderPrimitiveType::DOUBLE:
                            add<Mat4d>(name);
                            break;
                        default:
                            throw std::runtime_error("Unsupported matrix type");
                    }
                    break;
            }
        }

        size_t getOffset(const std::string &name) const {
            return offsets.at(name);
        }

        size_t getTotalSize() const {
            return totalSize;
        }

        const rg::ShaderStructType &getStructType() const {
            return structDef;
        }

    private:
        size_t totalSize = 0;
        std::unordered_map<std::string, size_t> offsets;
        rg::ShaderStructType structDef;
    };
}

#endif //XENGINE_LAYOUTSTD140_HPP
