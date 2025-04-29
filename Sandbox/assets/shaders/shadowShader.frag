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
};
layout(binding = 1) readonly buffer PointLightBuffer {
    LightAttributes lightsArray[];
};
layout(push_constant) uniform PushConstantBlock {
    int lightIndex;
} lightIndexObj;


layout(location = 0) in vec3 FragPosLight;
//layout(location = 1) in flat int lightIndex;
void main()
{
    float depth = 1.0; // Default value (far plane)
    //int lightID = int(floor(lightIndex + 0.5)); // Ensures proper integer conversion
    if (lightIndexObj.lightIndex ==0)
    {
        vec3 lightToFrag = FragPosLight - lightsArray[0].pl_position;
        depth = length(lightToFrag) / 100.0;
   }
   else if (lightIndexObj.lightIndex ==1)
   {
       vec3 lightToFrag_1 = FragPosLight - lightsArray[1].pl_position;
       depth = length(lightToFrag_1) / 100.0;
   }
    
    gl_FragDepth = depth;
}