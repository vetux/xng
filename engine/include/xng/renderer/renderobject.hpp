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

#ifndef XENGINE_RENDEROBJECT_HPP
#define XENGINE_RENDEROBJECT_HPP

namespace xng {
    /**
     * The scene will use ID-based explicit destruction to avoid pointer chasing for RenderObjects.
     */
    class RenderObject {
    public:
        typedef size_t ID;

        virtual ~RenderObject() = default;

        virtual bool isUploadComplete() {
            return true;
        }

        virtual void flush() {
        }
    };
}

#endif //XENGINE_RENDEROBJECT_HPP
