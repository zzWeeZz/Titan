#include "TNpch.h"
#include "GLTFImporter.h"
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE 
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include <filesystem>
#include <iostream>

#include "tiny_gltf.h"
#include "Titan/Core/Core.h"
#include <Titan/Rendering/Vertices.h>
#include <Titan/Assets/Model/Submesh.h>
namespace Titan
{
	void GLTFImporter::Import(const std::filesystem::path& filepath, std::vector<Submesh>& outMeshes)
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
		
		const tinygltf::Scene& scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); i++)
		{
			std::vector<Vertex> tempVertices;
			std::vector<uint32_t> tempIndex;
			const tinygltf::Node& node = model.nodes[scene.nodes[i]];
			LoadNode(node, model, tempVertices, tempIndex);
				
			// constructs with (std::Vector<vertex>&, std::Vector<uint32_t>&).
			outMeshes.emplace_back(tempVertices, tempIndex);
		}
	}
	void GLTFImporter::LoadNode(const tinygltf::Node& node, const tinygltf::Model& model, std::vector<Vertex>& outVerties, std::vector<uint32_t>& outIndices)
	{
		if (node.mesh > -1)
		{
			const tinygltf::Mesh mesh = model.meshes[node.mesh];

			for (size_t i = 0; i < mesh.primitives.size(); ++i)
			{
				const tinygltf::Primitive primative = mesh.primitives[i];

				const float* positionBuffer = nullptr;
				const float* normalBuffer = nullptr;
				const float* texCoordsBuffer = nullptr;
				const float* tangentBuffer = nullptr;

				size_t vertexCount = 0;

				if (primative.attributes.find("POSITION") != primative.attributes.end())
				{
					const tinygltf::Accessor& accessor = model.accessors[primative.attributes.find("POSITION")->second];
					const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
					positionBuffer = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

					vertexCount = accessor.count;
				}

				if (primative.attributes.find("NORMAL") != primative.attributes.end())
				{
					const tinygltf::Accessor& accessor = model.accessors[primative.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
					normalBuffer = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
				}

				if (primative.attributes.find("TEXCOORD_0") != primative.attributes.end())
				{
					const tinygltf::Accessor& accessor = model.accessors[primative.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
					texCoordsBuffer = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
				}

				if (primative.attributes.find("TANGENT") != primative.attributes.end())
				{
					const tinygltf::Accessor& accessor = model.accessors[primative.attributes.find("TANGENT")->second];
					const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
					tangentBuffer = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
				}

				outVerties.resize(vertexCount);
				for (size_t index = 0; index < vertexCount; ++index)
				{
					Vertex& vertex = outVerties[index];
					vertex.Position = positionBuffer ? *(glm::vec3*)&positionBuffer[index * 3] : glm::vec3();
					vertex.Normal = glm::normalize(normalBuffer ? *(glm::vec3*)&normalBuffer[index * 3] : glm::vec3(1.f));
					vertex.TexCoords = texCoordsBuffer ? *(glm::vec2*)&texCoordsBuffer[index * 2] : glm::vec2();
				}
				{
					const tinygltf::Accessor& accessor = model.accessors[primative.indices];
					const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

					switch (accessor.componentType)
					{
					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
					{
						const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++)
						{
							outIndices.emplace_back(buf[index]);
						}

						break;
					}

					case TINYGLTF_PARAMETER_TYPE_SHORT:
					{
						const int16_t* buf = reinterpret_cast<const int16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++)
						{
							outIndices.emplace_back(buf[index]);
						}
						break;
					}

					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
					{
						const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++)
						{
							outIndices.emplace_back(buf[index]);
						}
						break;
					}

					case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
					{
						const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						for (size_t index = 0; index < accessor.count; index++)
						{
							outIndices.emplace_back(buf[index]);
						}
						break;
					}

					default:
						TN_CORE_ERROR("Index component not supported!");
						return;
					}
				}
			}
		}
	}
}