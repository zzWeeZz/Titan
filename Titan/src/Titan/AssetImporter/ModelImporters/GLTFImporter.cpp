#include "GLTFImporter.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <filesystem>
#include <iostream>

#include "tiny_gltf.h"
#include "Titan/Core/Core.h"

namespace Titan
{
	void GLTFImporter::Import(const std::filesystem::path& filepath, Ref<VertexArray>& outVertex)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		if (filepath.extension() == ".glb")
		{
			bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filepath.string());
			if (!warn.empty())
			{
				TN_CORE_WARN("{0}", warn);
			}
			if (!err.empty())
			{
				TN_CORE_ERROR("{0}", err);
			}
			if (!ret)
			{
				TN_CORE_ERROR("Failed to load gltf file {0}", filepath.string());
				return;
			}
		}
		else if (filepath.extension() == ".gltf")
		{
			bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filepath.string());
			if (!warn.empty())
			{
				TN_CORE_WARN("{0}", warn);
			}
			if (!err.empty())
			{
				TN_CORE_ERROR("{0}", err);
			}
			if (!ret)
			{
				TN_CORE_ERROR("Failed to load gltf file {0}", filepath.string());
				return;
			}
		}
		std::vector<Vertex> tempVertices;
		std::vector<uint16_t> tempIndex;
		for (int i = 0; i < model.scenes.size(); i++)
		{
			const tinygltf::Scene& scene = model.scenes[i];
			for (int j = 0; j < scene.nodes.size(); j++)
			{
				const tinygltf::Node& node = model.nodes[scene.nodes[j]];
				if (node.mesh >= 0)
				{
					const tinygltf::Mesh& mesh = model.meshes[node.mesh];
					for (int k = 0; k < mesh.primitives.size(); k++)
					{
						const tinygltf::Primitive& primitive = mesh.primitives[k];
						if (primitive.indices >= 0)
						{
							const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
							const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
							const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

							const uint16_t* indices = reinterpret_cast<const uint16_t*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

							const tinygltf::Accessor& accessor2 = model.accessors[primitive.attributes.find("POSITION")->second];
							const tinygltf::BufferView& bufferView2 = model.bufferViews[accessor2.bufferView];
							const tinygltf::Buffer& buffer2 = model.buffers[bufferView2.buffer];
							const float* positions = reinterpret_cast<const float*>(&buffer2.data[bufferView2.byteOffset + accessor2.byteOffset]);


							const tinygltf::Accessor& accessor3 = model.accessors[primitive.attributes.find("NORMAL")->second];
							const tinygltf::BufferView& bufferView3 = model.bufferViews[accessor3.bufferView];
							const tinygltf::Buffer& buffer3 = model.buffers[bufferView3.buffer];
							const float* normals = reinterpret_cast<const float*>(&buffer3.data[bufferView3.byteOffset + accessor3.byteOffset]);

							const tinygltf::Accessor& accessor4 = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
							const tinygltf::BufferView& bufferView4 = model.bufferViews[accessor4.bufferView];
							const tinygltf::Buffer& buffer4 = model.buffers[bufferView4.buffer];
							const float* texCoord = reinterpret_cast<const float*>(&buffer4.data[bufferView4.byteOffset + accessor4.byteOffset]);
							for (size_t i = 0; i < accessor2.count; ++i) {
								// Positions are Vec3 components, so for each vec3 stride, offset for x, y, and z.
								Vertex vertex;
								vertex.Position.x = positions[i * 3 + 0];
								vertex.Position.y = positions[i * 3 + 1];
								vertex.Position.z = positions[i * 3 + 2];
								vertex.Normal.x = normals[i * 3 + 0];
								vertex.Normal.y = normals[i * 3 + 1];
								vertex.Normal.z = normals[i * 3 + 2];
								vertex.TexCoords = glm::vec2(texCoord[i * 2 + 0], texCoord[i * 2 + 1]);
								tempVertices.push_back(vertex);
							}
							for (size_t i = 0; i < accessor.count; ++i)
							{
								tempIndex.push_back(indices[i]);
							}
						}
					}
				}
			}
		}

		outVertex = VertexArray::Create(tempVertices, tempIndex);

		TN_CORE_INFO("Done Loading {0}", filepath.string());
	}
}
