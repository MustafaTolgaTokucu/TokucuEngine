#version 450


layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

struct ShadowUBO {
    mat4 shadowMatrices[6];
    
}; 
layout(binding = 2) readonly buffer PointLightBuffer {
	ShadowUBO lightMatrixArray[];  // Array of point lights
};


layout(push_constant) uniform PushConstantBlock {
    int lightIndex;
} lightIndexObj;

layout(location = 0) in vec3 FragPos[];
layout(location = 0) out vec3 FragPosLight;
void main() {
    for (int face = 0; face < 6; face++) // Each light has 6 faces
    {
        gl_Layer = (lightIndexObj.lightIndex * 6) + face; // Map correct layer
        for (int i = 0; i < 3; i++)
        {
            FragPosLight = FragPos[i];
            // Select the correct light matrix
            gl_Position = lightMatrixArray[lightIndexObj.lightIndex].shadowMatrices[face] * vec4(FragPos[i], 1.0);
            EmitVertex();
        }
        EndPrimitive();
    }
}