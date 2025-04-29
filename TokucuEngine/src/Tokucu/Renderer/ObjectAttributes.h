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
	

}