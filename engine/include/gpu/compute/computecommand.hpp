/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_COMPUTECOMMAND_HPP
#define XENGINE_COMPUTECOMMAND_HPP

namespace xng {
    struct ComputeCommand {
        Vector3<uint> num_groups; // The number of work groups in each dimension, cannot be zero.
        std::vector<ShaderBinding> bindings; // The bindings accessible to the shader with explicit binding
    };
}

#endif //XENGINE_COMPUTECOMMAND_HPP
