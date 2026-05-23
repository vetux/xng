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

#ifndef XENGINE_FONTMETRICS_HPP
#define XENGINE_FONTMETRICS_HPP

namespace xng {
    struct FontMetrics {
        int ascender; // The distance between baseline and the largest top value in pixels.
        int descender; // The distance between the baseline and the lowest bottom value in pixels.
        int height; // The suggested distance between baselines.

        FontMetrics() = default;

        FontMetrics(const int ascender, const int descender, const int height)
            : ascender(ascender), descender(descender), height(height) {
        }
    };
}

#endif //XENGINE_FONTMETRICS_HPP
