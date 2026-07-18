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

#ifndef XENGINE_RENDERMATERIAL_HPP
#define XENGINE_RENDERMATERIAL_HPP


#include "xng/renderer/shadingmodel.hpp"

namespace xng {
    class MaterialObject {
    public:
        MaterialObject(ColorRGBA albedoColor,
                       float metallicColor,
                       float roughnessColor,
                       float ambientOcclusionColor,
                       RenderObjectHandle<RenderTexture> _albedo,
                       const SamplingProperties &albedoProperties,
                       RenderObjectHandle<RenderTexture> _metallic,
                       const SamplingProperties &metallicProperties,
                       RenderObjectHandle<RenderTexture> _roughness,
                       const SamplingProperties &roughnessProperties,
                       RenderObjectHandle<RenderTexture> _ambientOcclusion,
                       const SamplingProperties &ambientOcclusionProperties,
                       RenderObjectHandle<RenderTexture> _normal,
                       const SamplingProperties &normalProperties,
                       const float normalIntensity,
                       const bool flipNormal,
                       const bool receiveShadows);



    private:
        ShadingModel shadingModel;
    };
}

#endif //XENGINE_RENDERMATERIAL_HPP