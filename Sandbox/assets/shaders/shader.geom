// Main Pass Geometry Shader
#version 450
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out; // Only output one triangle

layout(binding = 2) uniform ShadowUBO {
    mat4 shadowMatrices[6];
    float farPlane;
} shadowUBO;

// Inputs from vertex shader
layout(location = 0) in vec3 FragPos[]; 
layout(location = 1) in vec3 Normal[]; 
layout(location = 2) in vec3 v_Position[];
layout(location = 3) in vec2 TexCoords[];

// Outputs to fragment shader
layout(location = 0) out vec3 FragPosGeo; 
layout(location = 1) out vec3 NormalGeo; 
layout(location = 2) out vec3 v_PositionGeo;
layout(location = 3) out vec2 TexCoordsGeo;
layout(location = 4) out vec4 FragPosLight;



void main() {
    for(int i = 0; i < 3; ++i) {
        FragPosGeo = FragPos[i];
        NormalGeo = Normal[i];
        v_PositionGeo = v_Position[i];
        TexCoordsGeo = TexCoords[i];
        FragPosLight = shadowUBO.shadowMatrices[0] * vec4(FragPosGeo, 1.0); // Use first matrix for main pass
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }     
    EndPrimitive();
}
