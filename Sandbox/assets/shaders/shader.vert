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

layout(location = 0) out vec3 FragPos; 
layout(location = 1) out vec3 T;
layout(location = 2) out vec3 B;
layout(location = 3) out vec3 N;
layout(location = 4) out vec2 TexCoords;

void main() {
	TexCoords = inTexCoord;
    T = normalize(vec3(ubo.model * vec4(inTangent, 0.0)));
    N = normalize(vec3(ubo.model * vec4(inNormal, 0.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    B = cross(N, T);
    
    FragPos = vec3(ubo.model * vec4(inPosition, 1.0));
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
}