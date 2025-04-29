#version 450

layout(binding = 2) uniform sampler2D  skybox;

layout(location = 0) in vec3 texCoords;

layout(location = 0) out vec4 outColor;



const vec2 invAtan = vec2(0.1591, 0.3183);

// Function to convert 3D direction vector to equirectangular UV coordinates
vec2 SampleSphericalMap(vec3 direction)
{
    // Convert direction vector to spherical coordinates
    // atan(y, x) gives the azimuthal angle in [-pi, pi]
    // asin(z) gives the polar angle in [-pi/2, pi/2]
    vec2 uv = vec2(atan(direction.z, direction.x), asin(direction.y));
    
    // Map from spherical coordinates to UV:
    // U = (atan(z, x) + pi) / (2pi)  [0, 1]
    // V = (asin(y) + pi/2) / pi      [0, 1]
    uv *= invAtan;
    uv += 0.5;
    uv.y = 1.0 - uv.y;
    return uv;
}

void main() {

    // Normalize the world position to get a direction vector
    vec3 direction = normalize(texCoords);
    
    // Convert 3D direction to 2D UV coordinates for equirectangular texture
    vec2 uv = SampleSphericalMap(direction);

    float gamma = 1.8;
    // Sample the equirectangular texture
    vec3 color =pow( texture(skybox, uv).rgb,vec3(gamma));
    
    // Apply HDR tone mapping (simple Reinhard operator)
    color = color / (color + vec3(1.0));
    
    // Apply gamma correction
   // color = pow(color, vec3(1.0/2.2));
    
    outColor = vec4(color, 1.0);
}