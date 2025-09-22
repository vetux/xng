/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_SHADERENVIRONMENT_HPP
#define XENGINE_SHADERENVIRONMENT_HPP

namespace xng {
    enum ShaderEnvironment : int {
        ENVIRONMENT_NONE = 0,
        ENVIRONMENT_OPENGL,
        ENVIRONMENT_VULKAN_1_0,
        ENVIRONMENT_VULKAN_1_1,
        ENVIRONMENT_VULKAN_1_2,
        ENVIRONMENT_VULKAN_1_3,
    };
}

#endif //XENGINE_SHADERENVIRONMENT_HPP
