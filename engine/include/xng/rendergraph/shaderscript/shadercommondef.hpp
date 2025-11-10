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

#ifndef XENGINE_SHADERCOMMONDEF_HPP
#define XENGINE_SHADERCOMMONDEF_HPP

#define GetGenerator(_type0, _name0, \
    _type1, _name1, \
    _type2, _name2, \
    _type3, _name3, \
    _type4, _name4, \
    _type5, _name5, \
    _type6, _name6, \
    _type7, _name7, \
    _type8, _name8, \
    _type9, _name9, \
    _type10, _name10, \
    _type11, _name11, \
    _type12, _name12, \
    _type13, _name13, \
    _type14, _name14, \
    _type15, _name15, \
    _type16, _name16, \
    _type17, _name17, \
    _type18, _name18, \
    _type19, _name19, \
    _type20, _name20, \
    _type21, _name21, \
    _type22, _name22, \
    _type23, _name23, \
    _type24, _name24, \
    _type25, _name25, \
    _type26, _name26, \
    _type27, _name27, \
    _type28, _name28, \
    _type29, _name29, \
    _type30, _name30, \
    _type31, _name31, \
    _type32, _name32, \
    _type33, _name33, \
    _type34, _name34, \
    _type35, _name35, \
    _type36, _name36, \
    _type37, _name37, \
    _type38, _name38, \
    _type39, _name39, \
    _type40, _name40, \
    _type41, _name41, \
    _type42, _name42, \
    _type43, _name43, \
    _type44, _name44, \
    _type45, _name45, \
    _type46, _name46, \
    _type47, _name47, \
    _type48, _name48, \
    _type49, _name49, \
    NAME, ...) NAME

// In MSVC preprocessor __VA_ARGS__ is treated as a single token in macro argument lists.
// According to this: https://stackoverflow.com/a/32400131
// when wrapping __VA_ARGS__ usage in another macro invocation, it forces the preprocessor to not treat __VA_ARGS__ as a single token.
#define ExpandVAArgs(v) v

#endif //XENGINE_SHADERCOMMONDEF_HPP
