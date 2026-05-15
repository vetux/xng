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

#ifndef XENGINE_RENDERGRAPH_SHADERATTRIBUTELAYOUT_HPP
#define XENGINE_RENDERGRAPH_SHADERATTRIBUTELAYOUT_HPP

#include <vector>

#include "xng/rendergraph/shader/shaderdatatype.hpp"

namespace xng::rg {
    struct ShaderAttributeLayout {
        enum InterpolationMode {
            INTERPOLATE_SMOOTH,
            INTERPOLATE_NO_PERSPECTIVE,
            INTERPOLATE_FLAT
        };

        ShaderAttributeLayout() = default;

        explicit ShaderAttributeLayout(const std::vector<std::pair<std::string, ShaderPrimitiveType> > &namedElements) {
            for (auto &pair: namedElements) {
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
                ret += attr.getSize();
            return ret;
        }

        const std::string &getElementName(size_t index) const {
            return elementIdentifiers.at(index);
        }

        bool checkElement(const std::string &name) const {
            for (auto &attr: elementIdentifiers) {
                if (attr == name) {
                    return true;
                }
            }
            return false;
        }

        const std::vector<ShaderPrimitiveType> &getElements() const {
            return elements;
        }

        const std::vector<InterpolationMode> &getInterpolationModes() const {
            return interpolationModes;
        }

        void addElement(const std::string &name,
                        const ShaderPrimitiveType type,
                        const InterpolationMode interpolationMode = INTERPOLATE_SMOOTH) {
            elementIdentifiers.emplace_back(name);
            elements.emplace_back(type);
            interpolationModes.emplace_back(interpolationMode);
        }

    private:
        std::vector<std::string> elementIdentifiers;
        std::vector<ShaderPrimitiveType> elements;
        std::vector<InterpolationMode> interpolationModes;
    };
}

#endif //XENGINE_RENDERGRAPH_SHADERATTRIBUTELAYOUT_HPP
