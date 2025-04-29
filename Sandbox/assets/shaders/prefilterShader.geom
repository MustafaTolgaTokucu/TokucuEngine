#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

layout(binding = 1) uniform cubemapPosBuffer{
       mat4 cubemapPosArray[6];
};


layout(location = 0) in vec3 FragPos[];
layout(location = 0) out vec3 FragPosUVW;


void main() {
    for (int face = 0; face < 6; face++) // Each light has 6 faces
    {
        gl_Layer =  face; // Map correct layer
        for (int i = 0; i < 3; i++)
        {
            FragPosUVW = FragPos[i];
            // Select the correct light matrix
            gl_Position = cubemapPosArray[face] * vec4(FragPos[i], 1.0);
            EmitVertex();
        }
        EndPrimitive();
    }
}