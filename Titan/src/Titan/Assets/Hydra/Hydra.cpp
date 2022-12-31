#include "TNpch.h"
#include "Hydra.h"

#include <execution>

#include <meshoptimizer/src/meshoptimizer.h>

#include "Titan/Assets/Model/Submesh.h"

#include "Titan/Assets/Importers/ModelImporters/FBXImporter.h"
#include "Titan/Assets/Importers/ModelImporters/GLTFImporter.h"


namespace Titan
{
	void Hydra::ImportModel(const std::filesystem::path& filepath, std::vector<Submesh>& submeshes)
	{
		if (!std::filesystem::exists(filepath))
		{
			TN_CORE_ERROR("(Hydra::ImportModel) Could not find file {0}.", filepath.string());
			return;
		}
		const auto& extension = filepath.extension();
		if (extension == ".fbx")
		{
			FBXImporter::Import(filepath, submeshes);
		}
		else if (extension == ".gltf" || extension == ".glb")
		{
			GLTFImporter::Import(filepath, submeshes);
		}
		else
		{
			TN_CORE_ERROR("(Hydra::ImportModel) Extension: [{0}] is not supported model format!");
			return;
		}

		std::for_each(std::execution::par, submeshes.begin(), submeshes.end(), [&](Submesh& a)
			{
				GenerateMeshlets(a);
			});
			/*for (size_t i = 0; i < submeshes.size(); ++i)
			{
				GenerateMeshlets(submeshes[i]);
			}*/
	}
	void Hydra::GenerateMeshlets(Submesh& submesh)
	{
		std::vector<uint32_t> indices(submesh.GetIndices().size());

		meshopt_optimizeOverdraw(indices.data(), submesh.GetIndices().data(), submesh.GetIndices().size(), &submesh.GetRawVertices()[0].Position.x, submesh.GetRawVertices().size(), sizeof(RawVertex), 1.05f);

		for (size_t i = 0; i < indices.size(); ++i)
		{
			submesh.GetIndices()[i] = indices[i];
		}

		TN_CORE_INFO("generating meshlets");
		const size_t maxVertices = 64u;
		const size_t maxTriangles = 124u;
		const float coneWeight = 0.0f;

		size_t maxMeshLets = meshopt_buildMeshletsBound(submesh.GetIndices().size(), maxVertices, maxTriangles);

		std::vector<meshopt_Meshlet> meshlets(maxMeshLets);
		std::vector<uint32_t> meshletVertices(maxMeshLets * maxVertices);
		std::vector<uint8_t> meshletTriangle(maxMeshLets * maxTriangles * 3);

		size_t meshletCount = meshopt_buildMeshlets(
			meshlets.data(),
			meshletVertices.data(), meshletTriangle.data(),
			submesh.GetIndices().data(), submesh.GetIndices().size(),
			&submesh.GetRawVertices()[0].Position.x, submesh.GetRawVertices().size(), sizeof(RawVertex),
			maxVertices, maxTriangles, coneWeight);

		

		const meshopt_Meshlet& last = meshlets[meshletCount - 1];
		meshletVertices.resize(static_cast<size_t>(last.vertex_offset + last.vertex_count));
		meshletTriangle.resize(static_cast<size_t>(last.triangle_offset + ((last.triangle_count * 3 + 3) & ~3)));
		meshlets.resize(meshletCount);
		submesh.GetMeshlets().resize(meshletCount);
		submesh.GetMeshletVertices().resize(meshletVertices.size());
		submesh.GetIndices().resize(meshletTriangle.size());
		for (size_t i = 0; i < meshletTriangle.size(); ++i)
		{
			submesh.GetIndices()[i] = static_cast<uint32_t>(meshletTriangle[i]);
		}
		for (size_t i = 0; i < meshletVertices.size(); ++i)
		{
			submesh.GetMeshletVertices()[i] = (meshletVertices[i]);
		}
		TN_CORE_INFO("meshlets count -> {}", meshletCount);
		for (size_t i = 0; i < meshletCount; ++i)
		{
			submesh.GetMeshlets()[i].vertexOffset = meshlets[i].vertex_offset;
			submesh.GetMeshlets()[i].triangleOffset = meshlets[i].triangle_offset;
			submesh.GetMeshlets()[i].vertexCount = meshlets[i].vertex_count;
			submesh.GetMeshlets()[i].triangleCount = meshlets[i].triangle_count;
		}
		TN_CORE_INFO("meshlets generated");
		submesh.CreateBuffers();
	}
}