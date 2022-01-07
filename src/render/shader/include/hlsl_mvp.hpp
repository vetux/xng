/**
 *  Mana - 3D Game Engine
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

#ifndef MANA_HLSL_MVP_HPP
#define MANA_HLSL_MVP_HPP

static const char *HLSL_MVP = R"###(
    float4x4 MANA_M;
    float4x4 MANA_V;
    float4x4 MANA_P;
    float4x4 MANA_MVP;
    float4x4 MANA_M_INVERT;
    float4x4 MANA_VIEW_TRANSLATION;
)###";

#endif //MANA_HLSL_MVP_HPP
