// Adapted from https://learnopengl.com/code_viewer_gh.php?code=src/6.pbr/1.1.lighting/1.1.pbr.fs
// TODO: Implement pbr spot and directional lights
// TODO: Implement pbr IBL ambient lighting

#include "pi.glsl"

struct PBRPointLight {
    vec4 position;
    vec4 color;
    vec4 farPlane;
};

struct PBRDirectionalLight {
    vec4 direction;
    vec4 color;
    vec4 farPlane;
};

struct PBRSpotLight {
    vec4 position;
    vec4 direction_quadratic;
    vec4 color;
    vec4 farPlane;
    vec4 cutOff_outerCutOff_constant_linear;
};

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

struct PbrPass {
    vec3 N;
    vec3 V;
    vec3 F0;

    vec3 WorldPos;
    vec3 Normal;

// Material
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;

    vec3 camPos;
};

// Begin a pbr lighting pass
PbrPass pbr_begin(vec3 WorldPos,
vec3 Normal,
vec3 albedo,
float metallic,
float roughness,
float ao,
vec3 camPos) {
    PbrPass ret;

    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    ret.N = N;
    ret.V = V;
    ret.F0 = F0;

    ret.WorldPos = WorldPos;
    ret.Normal = Normal;
    ret.albedo = albedo;
    ret.metallic = metallic;
    ret.roughness = roughness;
    ret.ao = ao;
    ret.camPos = camPos;

    return ret;
}

// Calculate the reflectance of a point light using the pass returned by pbr_begin() and the accumulated reflectance, the returned reflectance is the passed reflectance with the light influence added
vec3 pbr_point(PbrPass pass, vec3 Lo, PBRPointLight light, float shadow) {
    vec3 lightPosition = light.position.xyz;
    vec3 lightColor = light.color.xyz;

    vec3 N = pass.N;
    vec3 V = pass.V;
    vec3 F0 = pass.F0;
    vec3 WorldPos= pass.WorldPos;
    vec3 Normal= pass.Normal;
    vec3 albedo= pass.albedo;
    float metallic= pass.metallic;
    float roughness= pass.roughness;
    float ao= pass.ao;
    vec3 camPos= pass.camPos;

    // calculate per-light radiance
    vec3 L = normalize(lightPosition - WorldPos);
    vec3 H = normalize(V + L);
    float distance = length(lightPosition - WorldPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;// + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadow;// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    return Lo;
}

// Calculate the reflectance of a directional light using the pass returned by pbr_begin() and the accumulated reflectance, the returned reflectance is the passed reflectance with the light influence added
vec3 pbr_directional(PbrPass pass, vec3 Lo, PBRDirectionalLight light, float shadow) {
    vec3 lightColor = light.color.xyz;
    vec3 lightDirection = -light.direction.xyz;

    vec3 N = pass.N;
    vec3 V = pass.V;
    vec3 F0 = pass.F0;
    vec3 WorldPos= pass.WorldPos;
    vec3 Normal= pass.Normal;
    vec3 albedo= pass.albedo;
    float metallic= pass.metallic;
    float roughness= pass.roughness;
    float ao= pass.ao;
    vec3 camPos= pass.camPos;

    // calculate per-light radiance

    vec3 L = normalize(lightDirection);
    vec3 H = normalize(V + L);

    float angle = acos(dot(normalize(lightDirection), normalize(Normal)));

    vec3 radiance = lightColor * cos(angle);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;// + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadow;// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    return Lo;
}

// Calculate the reflectance of a directional light using the pass returned by pbr_begin() and the accumulated reflectance, the returned reflectance is the passed reflectance with the light influence added
vec3 pbr_spot(PbrPass pass, vec3 Lo, PBRSpotLight light, float shadow) {
    vec3 lightPosition = light.position.xyz;
    vec3 lightColor = light.color.xyz;
    vec3 lightDirection = light.direction_quadratic.xyz;

    vec3 N = pass.N;
    vec3 V = pass.V;
    vec3 F0 = pass.F0;
    vec3 WorldPos= pass.WorldPos;
    vec3 Normal= pass.Normal;
    vec3 albedo= pass.albedo;
    float metallic= pass.metallic;
    float roughness= pass.roughness;
    float ao= pass.ao;
    vec3 camPos= pass.camPos;

    vec3 lightDir = normalize(lightPosition - WorldPos);

    float theta = dot(lightDir, normalize(-lightDirection));
    float epsilon = (light.cutOff_outerCutOff_constant_linear.x - light.cutOff_outerCutOff_constant_linear.y);
    float intensity = clamp((theta - light.cutOff_outerCutOff_constant_linear.y) / epsilon, 0.0, 1.0);

    vec3 L = normalize(lightPosition - WorldPos);
    vec3 H = normalize(V + L);
    float distance = length(lightPosition - WorldPos);
    float attenuation = 1.0 / (light.cutOff_outerCutOff_constant_linear.z + light.cutOff_outerCutOff_constant_linear.w * distance + light.direction_quadratic.w * (distance * distance));

    vec3 radiance = lightColor * attenuation * intensity;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;// + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;

    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadow;// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    return Lo;
}

vec3 pbr_finish(PbrPass pass, vec3 Lo)
{
    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * pass.albedo * pass.ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    //  color = color / (color + vec3(1.0));
    // gamma correct
    //  color = pow(color, vec3(1.0/2.2));

    return color;
}