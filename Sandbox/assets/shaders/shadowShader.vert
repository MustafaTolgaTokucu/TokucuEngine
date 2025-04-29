#version 450
//#extension GL_KHR_vulkan_glsl : enable
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


layout(location = 0) out vec3 FragPos;
void main() {
    // Transform vertex to world space
    vec4 worldPos = ubo.model * vec4(inPosition, 1.0);
    FragPos = worldPos.xyz;
    gl_Position = worldPos; // Let geometry shader handle view/projection
}