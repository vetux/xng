/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_FRAMEGRAPHSETTINGS_HPP
#define XENGINE_FRAMEGRAPHSETTINGS_HPP

#include <string>
#include <utility>

#include "xng/render/scenerenderersettings.hpp"

#define FRAMEGRAPH_SETTING(name, defaultValue) namespace xng { namespace FrameGraphSettings { static const SceneRendererSetting name = SceneRendererSetting(#name, typeid(defaultValue), defaultValue); } }

/**
 * User settings should be declared using the FRAMEGRAPH_SETTING macro in the top level namespace
 */

// float, Range(0, inf) Render resolution is backbuffer size * scale, if not defined the full backbuffer resolution is used, the rendered images are projected to fit the backbuffer resolution.
FRAMEGRAPH_SETTING(SETTING_RENDER_SCALE, static_cast<float>(1.0))

// int, the number of sub samples per pixel (MSAA) to use when forward rendering, if not defined the back buffer sample count is used.
FRAMEGRAPH_SETTING(SETTING_RENDER_SAMPLES, 1)

// Vec2i, The resolution of the point shadow maps
FRAMEGRAPH_SETTING(SETTING_SHADOW_MAPPING_POINT_RESOLUTION, Vec2i(2048, 2048))

#endif //XENGINE_FRAMEGRAPHSETTINGS_HPP
