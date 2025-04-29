#version 450

layout(binding = 1) uniform samplerCube skybox;

layout(location = 0) in vec3 texCoords;

layout(location = 0) out vec4 outColor;



void main(){
	vec3 envColor = textureLod(skybox, texCoords, 1.2).rgb; 
    outColor = vec4(envColor, 1.0);
	//outColor = texture(skybox, texCoords);
	//outColor = vec4(1.0, 0.0, 0.0, 1.0); // Bright red skybox
}
