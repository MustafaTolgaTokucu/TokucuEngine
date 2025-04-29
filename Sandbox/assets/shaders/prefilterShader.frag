#version 450
layout(binding = 2) uniform samplerCube environmentMaptest;
layout(push_constant) uniform PushConstantBlock {
    float roughness;
} roughnessUBO;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);

layout(location = 0) in vec3 FragPos;
layout(location = 0) out vec4 FragColor;
  
void main()
{		
   vec3 N = normalize(FragPos);    
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);     
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N,roughnessUBO.roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            prefilteredColor += texture(environmentMaptest, L).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
    //vec3 envColor = textureLod(environmentMap, FragPos, 1.2).rgb; 
    //FragColor = vec4(envColor, 1.0);
}  

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}  
//layout(binding = 2) uniform samplerCube environmentMap;
//
//const float PI = 3.14159265359;
//layout(location = 0) in vec3 FragPos;
//
//layout(location = 0) out vec4 FragColor;
//void main()
//{		
//	// The world vector acts as the normal of a tangent surface
//    // from the origin, aligned to WorldPos. Given this normal, calculate all
//    // incoming radiance of the environment. The result of this radiance
//    // is the radiance of light coming from -Normal direction, which is what
//    // we use in the PBR shader to sample irradiance.
//    vec3 N = normalize(FragPos);
//    
//    vec3 irradiance = vec3(0.0);   
//    
//    // tangent space calculation from origin point
//    vec3 up    = vec3(0.0, 1.0, 0.0);
//    vec3 right = normalize(cross(up, N));
//    up         = normalize(cross(N, right));
//       
//    float sampleDelta = 0.025;
//    float nrSamples = 0.0;
//    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
//    {
//        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
//        {
//            // spherical to cartesian (in tangent space)
//            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
//            // tangent space to world
//            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 
//    
//            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
//            nrSamples++;
//        }
//    }
//    irradiance = PI * irradiance * (1.0 / float(nrSamples));
//    
//    FragColor = vec4(irradiance, 1.0);
//   // FragColor=texture(environmentMap,FragPos);
//}