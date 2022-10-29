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

#ifndef XENGINE_FRAMEGRAPHSHADER_HPP
#define XENGINE_FRAMEGRAPHSHADER_HPP

#include <functional>
#include <string>

namespace xng::FrameGraphShader {
    /**
     * Defines the following headers:
     *
     * vertex.glsl - Shall be used by user vertex shaders.
     *
     * Input methods, call these methods to gather data in the vertex shader.
     * vec3 getPosition()
     * vec3 getNormal()
     * vec2 getUv()
     * vec3 getTangent()
     * vec3 getBitangent()
     * ivec4 getBoneIds()
     * vec4 getBoneWeights()
     * vec4 getInstanceRow0()
     * vec4 getInstanceRow1()
     * vec4 getInstanceRow2()
     * vec4 getInstanceRow3()
     *
     * Output methods, call these to output data from the vertex shader:
     * void setPosition(vec3 v)
     * void setNormal(vec3 v)
     * void setTangent(vec3 v)
     * void setUv(vec2 v)
     * void setWorldPosition(vec3 v)
     *
     *
     * fragment.glsl - Shall be used by user fragment shaders
     *
     * Input methods, call these to read data in the fragment shader:
     * vec3 getPosition()
     * vec3 getNormal()
     * vec3 getTangent()
     * float getRoughness()
     * float getMetallic()
     * float getAmbientOcclusion()
     * vec4 getAlbedo()
     * vec4 getAmbient()
     * vec4 getSpecular()
     * int getLightModel()
     * int getObjectID()
     * float getDepth()
     *
     * Output method, call this to set the output color of your fragment:
     * void setColor(vec4 color)
     * @return
     */
    XENGINE_EXPORT std::function<std::string(const char *)> getShaderInclude();
}

#endif //XENGINE_FRAMEGRAPHSHADER_HPP
