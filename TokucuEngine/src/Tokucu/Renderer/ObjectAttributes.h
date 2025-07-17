#pragma once
#include "glm/glm.hpp"
#include <vector>
namespace Tokucu {
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;  // Added Tangent
	};
	struct ModelData {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};
	struct ModelMesh {
		std::string name;             // Name of the mesh
		std::vector<Vertex> vertices; // Vertices for this part of the model
		std::vector<uint32_t> indices;// Indices for this part of the model
		std::string materialName;     // Name of the material assigned to this mesh
		std::vector< std::string> materialNames;             // Number of materials assigned to this mesh
		int materialIndex;            // Material index assigned to this mesh
	};


	struct FBXModelData {
		std::vector<ModelMesh> meshes; // List of all sub-meshes (each with a different material)
	};


	const std::vector<Vertex> secondVertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

			{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};
	const std::vector<uint32_t> secondIndices = {
	   0, 1, 2, 2, 3, 0,
	   4, 5, 6, 6, 7, 4
	};
	const std::vector<Vertex> cubeVertices{
		//Positions          // Normals           // Texture Coords
	   // Front face
	   {{-0.5f, -0.5f,  0.5f }  ,  {  0.0f,  0.0f,  1.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
	   {{ 0.5f, -0.5f,  0.5f }  ,  {  0.0f,  0.0f,  1.0f}   ,{  1.0f, 0.0f  }}, // Bottom-right
	   {{ 0.5f,  0.5f,  0.5f }  ,  {  0.0f,  0.0f,  1.0f}   ,{  1.0f, 1.0f  }}, // Top-right
	   {{-0.5f,  0.5f,  0.5f }  ,  {  0.0f,  0.0f,  1.0f}   ,{  0.0f, 1.0f  }}, // Top-left
	   // Back face		 
	   {{-0.5f, -0.5f, -0.5f }  ,  {  0.0f,  0.0f, -1.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
	   {{ 0.5f, -0.5f, -0.5f }  ,  {  0.0f,  0.0f, -1.0f}   ,{  1.0f, 0.0f  }}, // Bottom-right
	   {{ 0.5f,  0.5f, -0.5f }  ,  {  0.0f,  0.0f, -1.0f}   ,{  1.0f, 1.0f  }}, // Top-right
	   {{-0.5f,  0.5f, -0.5f }  ,  {  0.0f,  0.0f, -1.0f}   ,{  0.0f, 1.0f  }}, // Top-left
	   // Left face		
	   {{-0.5f, -0.5f, -0.5f }  ,  { -1.0f,  0.0f,  0.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
	   {{-0.5f,  0.5f, -0.5f }  ,  { -1.0f,  0.0f,  0.0f}   ,{  1.0f, 0.0f  }}, // Top-left
	   {{-0.5f,  0.5f,  0.5f }  ,  { -1.0f,  0.0f,  0.0f}   ,{  1.0f, 1.0f  }}, // Top-right
	   {{-0.5f, -0.5f,  0.5f }  ,  { -1.0f,  0.0f,  0.0f}   ,{  0.0f, 1.0f  }}, // Bottom-right
	   // Right face		
	   {{ 0.5f, -0.5f, -0.5f }  ,  {  1.0f,  0.0f,  0.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
	   {{ 0.5f,  0.5f, -0.5f }  ,  {  1.0f,  0.0f,  0.0f}   ,{  1.0f, 0.0f  }}, // Top-left
	   {{ 0.5f,  0.5f,  0.5f }  ,  {  1.0f,  0.0f,  0.0f}   ,{  1.0f, 1.0f  }}, // Top-right
	   {{ 0.5f, -0.5f,  0.5f }  ,  {  1.0f,  0.0f,  0.0f}   ,{  0.0f, 1.0f  }}, // Bottom-right
	   //Top face		
	   {{ -0.5f,  0.5f, -0.5f}  ,  {  0.0f,  1.0f,  0.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
	   {{  0.5f,  0.5f, -0.5f}  ,  {  0.0f,  1.0f,  0.0f}   ,{  1.0f, 0.0f  }}, // Bottom-right
	   {{  0.5f,  0.5f,  0.5f}  ,  {  0.0f,  1.0f,  0.0f}   ,{  1.0f, 1.0f  }}, // Top-right
	   {{ -0.5f,  0.5f,  0.5f}  ,  {  0.0f,  1.0f,  0.0f}   ,{  0.0f, 1.0f  }}, // Top-left
	   // Bottom face	 	
	   {{ -0.5f, -0.5f, -0.5f}  ,  {  0.0f, -1.0f,  0.0f}   ,{  0.0f, 0.0f  }}, // Bottom-left
	   {{  0.5f, -0.5f, -0.5f}  ,  {  0.0f, -1.0f,  0.0f}   ,{  1.0f, 0.0f  }}, // Bottom-right
	   {{  0.5f, -0.5f,  0.5f}  ,  {  0.0f, -1.0f,  0.0f}   ,{  1.0f, 1.0f  }}, // Top-right
	   {{ -0.5f, -0.5f,  0.5f}  ,  {  0.0f, -1.0f,  0.0f}   ,{  0.0f, 1.0f  }}  // Top-left
	};
	const std::vector<uint32_t> cubeIndices{
		// Front face (CCW)
		1, 2, 0,  2, 3, 0,
		// Back face (CCW)
		6, 5, 4,  7, 6, 4,
		// Left face (CCW)
		8, 10, 9,  8, 11, 10,
		// Right face (CCW)
		12, 13, 14,  12, 14, 15,
		// Top face (CCW)
		16, 18, 17,  16, 19, 18,
		// Bottom face (CCW)
		20, 21, 22,  20, 22, 23
	};

}