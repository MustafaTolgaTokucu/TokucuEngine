#version 450

struct LightAttributes {
        vec3 pl_color;
		vec3 pl_position;
		vec3 pl_ambient;
		vec3 pl_diffuse;
		vec3 pl_specular;
		vec3 pl_viewpos;
		float pl_constant;
		float pl_linear;
		float pl_quadratic;
		float pl_pointlightNumber;
};
layout(binding = 1) readonly buffer PointLightBuffer {
	LightAttributes lightsArray[];  // Array of point lights
};
const float PI = 3.14159265359;

layout(binding = 3) uniform sampler2D u_ambientTexture;
layout(binding = 4) uniform sampler2D u_diffuseTexture;
layout(binding = 5) uniform sampler2D u_specularTexture;
layout(binding = 6) uniform sampler2D u_normalMap;
layout(binding = 7) uniform samplerCubeArray u_shadowMap;
layout(binding = 8) uniform samplerCube u_irradianceMap;
layout(binding = 9) uniform samplerCube u_prefilterMap;
layout(binding = 10) uniform sampler2D u_brdfLUT;

layout(location = 0) in vec3 FragPos; 
layout(location = 1) in vec3 T;
layout(location = 2) in vec3 B;
layout(location = 3) in vec3 Nor;
layout(location = 4) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;

//Functions
vec3 CalcPointLight(LightAttributes light,vec3 normal, vec3 fragPos, vec3 viewDir, int index); 
float CalcShadow(LightAttributes light,vec3 normal, vec3 fragPos, int index);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float DistributionGGX(vec3 normal, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 normal, vec3 V, vec3 L, float roughness);

//Variables
vec3 F0 = vec3(0.04); 
vec3 Lo = vec3(0.0);
float shadow = 0.0f;

vec3 albedo = texture(u_ambientTexture, TexCoords).rgb;
float metallic=texture(u_specularTexture, TexCoords).b;
float roughness = texture(u_specularTexture, TexCoords).g;
float ao = texture(u_specularTexture, TexCoords).r;


//Main
void main() {
	vec3 normal;
        mat3 TBN = mat3(T,B,Nor);
        vec3 normalMapColor = texture(u_normalMap, TexCoords).rgb * 2.0 - 1.0;
        normal = normalize(TBN * normalMapColor);
    
    vec3 viewDir = normalize(lightsArray[0].pl_viewpos - FragPos);

    vec3 R = reflect(-viewDir, normal);
    //vec3 R = vec3(0.f);
    F0 = mix(F0, albedo, metallic);
    // Calculate contribution from each point light
   for(int i = 0; i < 2; i++) {
        Lo += CalcPointLight(lightsArray[i], normal, FragPos, viewDir,i);
        shadow += CalcShadow(lightsArray[i], normal, FragPos ,i);

   }
   //roughness = 1;
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
   // vec3 F=vec3(0.25);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
  
    vec3 irradiance = texture(u_irradianceMap, normal).rgb;
    vec3 diffuse      = irradiance * albedo;

    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    
    vec3 prefilteredColor = textureLod(u_prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;

    vec2 brdf  = texture(u_brdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F* brdf.x + brdf.y);

    vec3 ambient = (kD*diffuse +specular) * ao;
    vec3 color = ambient + Lo;
    //color =  ambient;
    //Not correct, just for better shadow visibility
    //color *= 1.f - shadow;
    color = color / (color + vec3(1.0));

    //color =(albedo);
    //Gamma correction
    //color = pow(color, vec3(1.0/2.2)); 
    outColor = vec4(color, 1.0);
}
 //Calculate point light contribution including shadows
vec3 CalcPointLight(LightAttributes light, vec3 normal, vec3 fragPos, vec3 viewDir, int index) {
	// FOR PBR
	vec3 L = normalize(light.pl_position - fragPos);
	vec3 H = normalize(L + viewDir);

	float distance    = length(light.pl_position - fragPos);
    float attenuation = 1.0 / (distance * distance);
    attenuation = 1.0 / (light.pl_constant + light.pl_linear * distance + light.pl_quadratic * (distance * distance));
	vec3 radiance = light.pl_color; 
    radiance*= attenuation;
    
    vec3 kS = vec3(0.0);
    vec3 kD = vec3(0.0);

    float NDF = DistributionGGX(normal, H, roughness);
    float G   = GeometrySmith(normal, viewDir, L, roughness);
    vec3 F  = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;


    kS = F;
    kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(normal, L), 0.0); 

    float shadow = CalcShadow(light, normal, fragPos ,index);

    vec3 result = (kD * albedo / PI + specular ) * radiance * NdotL;
    return result;
    //For proper shadow-light interraction
    //Lo *= (1 - shadow);

}
float CalcShadow(LightAttributes light, vec3 normal, vec3 fragPos, int index ) {
    vec3 fragToLight = fragPos - light.pl_position;
    float closestDepth = texture(u_shadowMap, vec4(fragToLight, index)).r * 100;
    float currentDepth = length(fragToLight);
	float bias = max(0.05 * (1.0 - dot(normal, fragToLight)), 0.005);
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    return shadow;
}
float DistributionGGX(vec3 normal, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(normal, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 normal, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(normal, V), 0.0);
    float NdotL = max(dot(normal, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
   return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  
