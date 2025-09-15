/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_SHADERATTRIBUTELAYOUT_HPP
#define XENGINE_SHADERATTRIBUTELAYOUT_HPP

#include <vector>

#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng {
    struct ShaderAttributeLayout {
        ShaderAttributeLayout() = default;

        explicit ShaderAttributeLayout(const std::vector<std::pair<std::string, ShaderDataType>>& namedElements) {
            for (auto &pair : namedElements) {
                elements.emplace_back(pair.second);
                elementIdentifiers.emplace_back(pair.first);
            }
        }

        bool operator==(const ShaderAttributeLayout &other) const {
            return elements == other.elements && elementIdentifiers == other.elementIdentifiers;
        }

        bool operator!=(const ShaderAttributeLayout &other) const {
            return !(*this == other);
        }

        size_t getLayoutSize() const {
            size_t ret = 0;
            for (auto &attr: elements)
                ret += attr.stride();
            return ret;
        }

        ShaderDataType getElementType(const std::string &attributeName) const {
            for (auto i = 0; i < elementIdentifiers.size(); ++i) {
                if (elementIdentifiers.at(i) == attributeName) {
                    return elements.at(i);
                }
            }
            throw std::runtime_error("No such attribute " + attributeName);
        }

        size_t getElementIndex(const std::string &attributeName) const {
            for (size_t i = 0; i < elementIdentifiers.size(); ++i) {
                if (elementIdentifiers.at(i) == attributeName) {
                    return i;
                }
            }
            throw std::runtime_error("No such attribute " + attributeName);
        }

        const std::string & getElementName(size_t index) const {
            return elementIdentifiers.at(index);
        }

        const std::vector<ShaderDataType> &getElements() const {
            return elements;
        }

        void addElement(const std::string &name, ShaderDataType type) {
            elementIdentifiers.emplace_back(name);
            elements.emplace_back(type);
        }

    private:
        std::vector<std::string> elementIdentifiers;
        std::vector<ShaderDataType> elements;
    };
}

#endif //XENGINE_SHADERATTRIBUTELAYOUT_HPP
