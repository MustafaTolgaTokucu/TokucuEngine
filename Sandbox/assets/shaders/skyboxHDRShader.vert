#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


layout(location = 0) out vec3 texCoords;
void main() {
    texCoords = inPosition;
    
    // Remove translation from view matrix
    mat4 viewRotOnly = mat4(
        vec4(ubo.view[0].xyz, 0.0),
        vec4(ubo.view[1].xyz, 0.0),
        vec4(ubo.view[2].xyz, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
    //gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    vec4 pos = ubo.proj * ubo.view* vec4(inPosition, 1.0);
    gl_Position = pos.xyww;  // Force depth to be 1.0 after perspective division
    //gl_Position = vec4(inPosition, 1.0);
    //gl_Position.z = 0; // Force it to pass depth test
}