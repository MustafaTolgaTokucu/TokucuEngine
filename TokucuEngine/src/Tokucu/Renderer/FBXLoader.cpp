#include "tkcpch.h"
#include "FBXLoader.h"

namespace Tokucu {

    FBXLoader::FBXLoader(const char* filepath)
	{
		LoadFBX(filepath);

	}
    FBXLoader::~FBXLoader() {

    }
    void FBXLoader::LoadFBX(const char* filepath) {
        // Create FBX Manager
        FbxManager* manager = FbxManager::Create();

        // Create IO settings
        FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
        manager->SetIOSettings(ios);

        // Create Importer
        FbxImporter* importer = FbxImporter::Create(manager, "");

        // Initialize the importer
        if (!importer->Initialize(filepath, -1, manager->GetIOSettings())) {
            std::cerr << "FBX Import failed: " << importer->GetStatus().GetErrorString() << std::endl;
            return;
        }
        // Create a new scene
        FbxScene* scene = FbxScene::Create(manager, "Scene");
        // Set import settings to triangulate during import
        ios->SetBoolProp(IMP_FBX_MATERIAL, true);
        ios->SetBoolProp(IMP_FBX_TEXTURE, false);
        ios->SetBoolProp(IMP_FBX_LINK, true);
        ios->SetBoolProp(IMP_FBX_SHAPE, true);
        ios->SetBoolProp(IMP_FBX_GOBO, true);
        ios->SetBoolProp(IMP_FBX_ANIMATION, false);
        ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
        ios->SetBoolProp(IMP_FBX_MODEL, true);
        ios->SetBoolProp("Import|IncludeGrp|Geometry|Mesh", true);
        ios->SetBoolProp("Import|IncludeGrp|Geometry|Triangulate", false);

        // Initialize only ONCE after setting properties
        if (!importer->Initialize(filepath, -1, ios)) {
            std::cerr << "FBX Import failed: " << importer->GetStatus().GetErrorString() << std::endl;
            importer->Destroy();
            manager->Destroy();
            return;
        }
        // Import FBX content into the scene
        importer->Import(scene);
       FbxGeometryConverter converter(scene->GetFbxManager());
       
       // Apply triangulation only when needed
       for (int i = 0; i < scene->GetNodeCount(); i++) {
           FbxNode* node = scene->GetNode(i);
           FbxMesh* mesh = node->GetMesh();
       
       
           if (mesh && NeedsTriangulation(mesh)) {
               converter.Triangulate(mesh, true);
           }
       }
        //FbxGeometryConverter(manager).Triangulate(scene, /*replace*/ true);
        // Process the scene (extract meshes, materials, etc.)
        ProcessFBXScene(scene);

        // Cleanup 
        importer->Destroy();
        manager->Destroy();
    }

    void FBXLoader::ProcessFBXScene(FbxScene* scene) {
        FbxNode* rootNode = scene->GetRootNode();
        if (rootNode) {
            for (int i = 0; i < rootNode->GetChildCount(); i++) {
                ProcessFBXNode(rootNode->GetChild(i));
            }
        }
    }

    void FBXLoader::ProcessFBXNode(FbxNode* node) {
        if (!node) return;
        FbxMesh* mesh = node->GetMesh();
        if (mesh) {
            
            ExtractMeshData(mesh);
        }
        for (int i = 0; i < node->GetChildCount(); i++) {
            ProcessFBXNode(node->GetChild(i));
        }
    }

    void FBXLoader::ExtractMeshData(FbxMesh* mesh) {
        int materialCount = mesh->GetNode()->GetMaterialCount();
        FbxLayerElementArrayTemplate<int>* materialIndices = nullptr;
        if (mesh->GetElementMaterial()) {
            materialIndices = &mesh->GetElementMaterial()->GetIndexArray();
        }
        std::vector<ModelMesh> materialMeshes(materialCount);
        // Process each polygon
        for (int polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); polygonIndex++) {
            int materialIndex = (materialIndices) ? materialIndices->GetAt(polygonIndex) : 0;
            ModelMesh& currentMesh = materialMeshes[materialIndex];
            // Process each vertex of the triangle
            for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
                int controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
                Vertex vertex;
                // Position
                FbxVector4 position = mesh->GetControlPointAt(controlPointIndex);
                FbxAMatrix globalTransform = mesh->GetNode()->EvaluateGlobalTransform();
                FbxVector4 transformedVertex = globalTransform.MultT(position);
                vertex.Position = glm::vec3(transformedVertex[0], transformedVertex[1], transformedVertex[2]);
                // Normal
                FbxVector4 normal;
                mesh->GetPolygonVertexNormal(polygonIndex, vertexIndex, normal);
                vertex.Normal = glm::vec3(normal[0], normal[1], normal[2]);
                // UV
                if (mesh->GetElementUVCount() > 0) {
                    FbxVector2 uv;
                    bool unmapped;
                    mesh->GetPolygonVertexUV(polygonIndex, vertexIndex, mesh->GetElementUV(0)->GetName(), uv, unmapped);
                    vertex.TexCoords = glm::vec2(uv[0], 1.0f - uv[1]);
                }
                currentMesh.vertices.push_back(vertex);
                currentMesh.indices.push_back(static_cast<uint32_t>(currentMesh.vertices.size() - 1));
            }
            // Get material name once per material
            if (currentMesh.materialNames.empty()) {
                FbxSurfaceMaterial* material = mesh->GetNode()->GetMaterial(materialIndex);
                if (material) {
                    //currentMesh.materialNames.push_back(material->GetName());
					currentMesh.materialName = material->GetName();
                }
            }
        }
        // Add non-empty meshes to model data
        for (auto& mesh : materialMeshes) {
            if (!mesh.vertices.empty()) {
                CalculateTangents(mesh);
                modelData.meshes.push_back(mesh);
            }
        }
    }

    void FBXLoader::CalculateTangents(ModelMesh& mesh) {
        for (size_t i = 0; i < mesh.indices.size(); i += 3) {
            Vertex& v0 = mesh.vertices[mesh.indices[i]];
            Vertex& v1 = mesh.vertices[mesh.indices[i + 1]];
            Vertex& v2 = mesh.vertices[mesh.indices[i + 2]];

            glm::vec3 edge1 = v1.Position - v0.Position;
            glm::vec3 edge2 = v2.Position - v0.Position;

            glm::vec2 deltaUV1 = v1.TexCoords - v0.TexCoords;
            glm::vec2 deltaUV2 = v2.TexCoords - v0.TexCoords;

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            glm::vec3 tangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            v0.Tangent += tangent;
            v1.Tangent += tangent;
            v2.Tangent += tangent;
        }

        // Normalize tangents
        for (auto& vertex : mesh.vertices) {
            vertex.Tangent = glm::normalize(vertex.Tangent);
        }
    }

    bool FBXLoader::NeedsTriangulation(FbxMesh* mesh) {
        for (int i = 0; i < mesh->GetPolygonCount(); i++) {
            if (mesh->GetPolygonSize(i) > 3) {
                return true;  // Quad or N-gon detected
            }
        }
        return false;
    }
}