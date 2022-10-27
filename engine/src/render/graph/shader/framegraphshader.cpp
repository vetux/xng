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

#include <stdexcept>

#include "render/graph/shader/framegraphshader.hpp"

static const char *SHADER_VERTEX = R"###(
// Layout bindings might be different on other pipelines.
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 boneWeights;
layout (location = 7) in vec4 instanceRow0;
layout (location = 8) in vec4 instanceRow1;
layout (location = 9) in vec4 instanceRow2;
layout (location = 10) in vec4 instanceRow3;

layout(location = 0) out vec3 fPos;
layout(location = 1) out vec3 fNorm;
layout(location = 2) out vec3 fTan;
layout(location = 3) out vec2 fUv;
layout(location = 4) out vec3 wPos;

// The stable include interface for user fragment shaders, These methods should be available on all pipelines.
vec3 getPosition() {
    return position;
}

vec3 getNormal() {
    return normal;
}

vec2 getUv() {
    return uv;
}

vec3 getTangent() {
    return tangent;
}

vec3 getBitangent() {
    return bitangent;
}

ivec4 getBoneIds() {
    return boneIds;
}

vec4 getBoneWeights() {
    return boneWeights;
}

vec4 getInstanceRow0() {
    return instanceRow0;
}

vec4 getInstanceRow1() {
    return instanceRow1;
}

vec4 getInstanceRow2() {
    return instanceRow2;
}

vec4 getInstanceRow3() {
    return instanceRow3;
}

void setPosition(vec3 v) {
    fPos = v;
}

void setNormal(vec3 v) {
    fNorm = v;
}

void setTangent(vec3 v) {
    fTan = v;
}

void setUv(vec2 v) {
    fUv = v;
}

void setWorldPosition(vec3 v) {
    wPos = v;
}
)###";

static const char *SHADER_FRAGMENT = R"###(
// Layout bindings might be different on other pipelines.
layout (location = 0) in vec4 fPosition;
layout (location = 1) in vec2 fUv;

layout (location = 0) out vec4 outputColor;

layout(binding = 0) uniform sampler2DMS texPosition;
layout(binding = 1) uniform sampler2DMS texNormal;
layout(binding = 2) uniform sampler2DMS texTangent;
layout(binding = 3) uniform sampler2DMS texRoughnessMetallicAo;
layout(binding = 4) uniform sampler2DMS texAlbedo;
layout(binding = 5) uniform sampler2DMS texAmbient;
layout(binding = 6) uniform sampler2DMS texSpecular;
layout(binding = 7) uniform isampler2DMS texLightmodelObject;
layout(binding = 8) uniform sampler2DMS texDepth;

// The stable include interface for user fragment shaders, These methods should be available on all pipelines.
vec3 getPosition() {
    return texelFetch(texPosition, fUv, gl_SampleID).xyz;
}

vec3 getNormal() {
    return texelFetch(texNormal, fUv, gl_SampleID).xyz;
}

vec3 getTangent() {
    return texelFetch(texTangent, fUv, gl_SampleID).xyz;
}

float getRoughness() {
    return texelFetch(texRoughnessMetallicAo, fUv, gl_SampleID).x;
}

float getMetallic() {
    return texelFetch(texRoughnessMetallicAo, fUv, gl_SampleID).y;
}

float getAmbientOcclusion() {
    return texelFetch(texRoughnessMetallicAo, fUv, gl_SampleID).z;
}

vec4 getAlbedo() {
    return texelFetch(texAlbedo, fUv, gl_SampleID);
}

vec4 getAmbient() {
    return texelFetch(texAmbient, fUv, gl_SampleID);
}

vec4 getSpecular() {
    return texelFetch(texSpecular, fUv, gl_SampleID);
}

int getLightModel() {
    return texelFetch(texLightmodelObject, fUv, gl_SampleID).x;
}

int getObjectID() {
    return texelFetch(texLightmodelObject, fUv, gl_SampleID).y;
}

float getDepth() {
    return texelFetch(texDepth, fUv, gl_SampleID).x;
}

void setColor(vec4 color) {
    outputColor = color;
}
)###";

namespace xng {
    std::function<std::string(const char *)> FrameGraphShader::getShaderInclude() {
        return [](const char *includeName) {
            if (std::string(includeName) == "vertex.glsl") {
                return SHADER_VERTEX;
            } else if (std::string(includeName) == "fragment.glsl") {
                return SHADER_FRAGMENT;
            } else {
                throw std::runtime_error("Invalid include name");
            }
        };
    }
}
