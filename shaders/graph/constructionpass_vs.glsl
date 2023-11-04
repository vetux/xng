#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUv;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

layout(location = 0) out vec3 fPos;
layout(location = 1) out vec3 fNorm;
layout(location = 2) out vec3 fTan;
layout(location = 3) out vec2 fUv;
layout(location = 4) out vec4 vPos;
layout(location = 5) out vec3 fT;
layout(location = 6) out vec3 fB;
layout(location = 7) out vec3 fN;
layout(location = 8) flat out uint drawID;

struct ShaderAtlasTexture {
    ivec4 level_index_filtering_assigned;
    vec4 atlasScale_texSize;
};

struct ShaderDrawData {
    mat4 model;
    mat4 mvp;

    ivec4 shadeModel_objectID_boneOffset_shadows;
    vec4 metallic_roughness_ambientOcclusion_shininess;

    vec4 diffuseColor;
    vec4 ambientColor;
    vec4 specularColor;

    vec4 normalIntensity;

    ShaderAtlasTexture normal;

    ShaderAtlasTexture metallic;
    ShaderAtlasTexture roughness;
    ShaderAtlasTexture ambientOcclusion;

    ShaderAtlasTexture diffuse;
    ShaderAtlasTexture ambient;
    ShaderAtlasTexture specular;
    ShaderAtlasTexture shininess;
};

layout(binding = 0, std140) buffer ShaderUniformBuffer
{
    ShaderDrawData data[];
} globs;

layout(binding = 1) uniform sampler2DArray atlasTextures[12];

void main()
{
    ShaderDrawData data = globs.data[gl_DrawID];

    vPos = data.mvp * vec4(vPosition, 1);
    fPos = (data.model * vec4(vPosition, 1)).xyz;
    fUv = vUv;

    fNorm = normalize(vNormal);
    fTan = normalize(vTangent);

    //https://www.gamedeveloper.com/programming/three-normal-mapping-techniques-explained-for-the-mathematically-uninclined
    fN = normalize((data.model * vec4(vNormal, 0.0)).xyz);
    fT = normalize((data.model * vec4(vTangent, 0.0)).xyz);
    fB = normalize((data.model * vec4(cross(vNormal, vTangent.xyz) * 1, 0.0)).xyz);

    gl_Position = vPos;

    drawID = gl_DrawID;
}
