#include "TNpch.h"
#include "FBXImporter.h"

#include "OpenFBX/src/ofbx.h"

#include "Titan/Rendering/Vertices.h"
#include "Titan/Assets/Model/Submesh.h"
#include "Titan/Utils/FilesystemUtils.h"

namespace Titan
{
	void FBXImporter::Import(const std::filesystem::path& filepath, std::vector<Submesh>& outVertex)
	{

		FILE* fp = fopen(filepath.string().c_str(), "rb");

		if (!fp) return;

		fseek(fp, 0, SEEK_END);
		long file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		auto* content = new ofbx::u8[file_size];
		fread(content, 1, file_size, fp);
		ofbx::IScene* scene = nullptr;
		scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
		if (!scene)
		{
			TN_CORE_ERROR("Could not load fbx file: {}. exited with error: {}", filepath.string(), ofbx::getError());
			delete[] content;
			content = nullptr;
			fclose(fp);
			return;
		}

		LoadScene(scene, outVertex);
		delete[] content;
		content = nullptr;
		fclose(fp);
	}
	void FBXImporter::LoadScene(ofbx::IScene* scene, std::vector<Submesh>& outSubmeshes)
	{
		int32_t objIdx = 0;
		int32_t indicesOffset = 0;
		int32_t normalsOffset = 0;
		int32_t meshCount = scene->getMeshCount();

		for(size_t i = 0; i < meshCount; ++i)
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			const ofbx::Mesh& mesh = *scene->getMesh(static_cast<int>(i));
			const ofbx::Geometry& geom = *mesh.getGeometry();

			int32_t vertexCount = geom.getVertexCount();

			const ofbx::Vec3* pos = geom.getVertices();
			const ofbx::Vec3* normals = nullptr;
			const ofbx::Vec2* uvs = nullptr;
			bool hasNormals = geom.getNormals() != nullptr;
			bool hasUVs = geom.getUVs() != nullptr;
			if (hasNormals)
			{
				normals = geom.getNormals();
			}
			if (hasUVs)
			{
				uvs = geom.getUVs();
			}

			vertices.resize(vertexCount);
			for (size_t v = 0; v < vertexCount; ++v)
			{
				auto& vert = vertices[v];
				vert.Position = *(glm::vec3*)&pos[v];
				vert.Normal = (hasNormals) ? *(glm::vec3*)&normals[v] : glm::vec3(1.f);
				vert.TexCoords = (hasUVs) ? *(glm::vec2*)&uvs[v] : glm::vec2(0.f);
			}

			const int32_t* faceIndices = geom.getFaceIndices();
			int32_t indexCount = geom.getIndexCount();
			bool newFace = true;
			for (size_t j = 0; j < indexCount; ++j)
			{
				if (newFace)
				{
					newFace = false;
				}

				int32_t idx = (faceIndices[i] < 0) ? -faceIndices[i] : (faceIndices[i] + 1);

				indices.emplace_back(indicesOffset + idx);
				newFace = faceIndices[i] < 0;
			}

			indicesOffset += vertexCount;
			normalsOffset += indicesOffset;

			// constructs with (std::Vector<vertex>&, std::Vector<uint32_t>&).
			outSubmeshes.emplace_back(vertices, indices);
		}
	}
}