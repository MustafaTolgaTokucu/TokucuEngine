#pragma once

#include <fbxsdk.h>
#include "ObjectAttributes.h"
namespace Tokucu
{
	class FBXLoader
	{
	public:
		
		FBXLoader(const char* filepath);
		~FBXLoader();
		void LoadFBX(const char* filepath);
		void ProcessFBXScene(FbxScene* scene);
		void ProcessFBXNode(FbxNode* node);
		void ExtractMeshData(FbxMesh* mesh);
		//void ExtractMaterial(FbxMesh* mesh);
		FBXModelData getModelData() { return modelData; }
		bool NeedsTriangulation(FbxMesh* mesh);
		void CalculateTangents(ModelMesh& mesh);

	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	
		//FbxAMatrix globalTransform;
		FBXModelData modelData;
	
	};
}


