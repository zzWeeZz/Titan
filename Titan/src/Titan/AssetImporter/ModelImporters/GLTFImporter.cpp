#include "GLTFImporter.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include "Titan/Core/Core.h"

namespace Titan
{
	void GLTFImporter::Import(const std::string& filepath)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);
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
			TN_CORE_ERROR("Failed to load gltf file {0}", filepath);
			return;
		}

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
							const std::string& bufferData = std::string(std::string::size_type(buffer.data.data()), buffer.data.size());
							const std::string& indicesData = std::string(bufferData.data() + bufferView.byteOffset + accessor.byteOffset, accessor.count * accessor.ByteStride(bufferView));
							TN_CORE_INFO("{0}", indicesData);

							const tinygltf::Accessor& accessor2 = model.accessors[primitive.attributes.find("POSITION")->second];
							const tinygltf::BufferView& bufferView2 = model.bufferViews[accessor2.bufferView];
							const tinygltf::Buffer& buffer2 = model.buffers[bufferView2.buffer];
							const std::string& bufferData2 = std::string(std::string::size_type(buffer2.data.data()), buffer2.data.size());
							const std::string& positionsData = std::string(bufferData2.data() + bufferView2.byteOffset + accessor2.byteOffset, accessor2.count * accessor2.ByteStride(bufferView2));
							TN_CORE_INFO("{0}", positionsData);

							const tinygltf::Accessor& accessor3 = model.accessors[primitive.attributes.find("NORMAL")->second];
							const tinygltf::BufferView& bufferView3 = model.bufferViews[accessor3.bufferView];
							const tinygltf::Buffer& buffer3 = model.buffers[bufferView3.buffer];
							const std::string& bufferData3 = std::string(std::string::size_type(buffer3.data.data()), buffer3.data.size());
							const std::string& normalsData = std::string(bufferData3.data() + bufferView3.byteOffset + accessor3.byteOffset, accessor3.count * accessor3.ByteStride(bufferView3));
							TN_CORE_INFO("{0}", normalsData);

							const tinygltf::Accessor& accessor4 = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
							const tinygltf::BufferView& bufferView4 = model.bufferViews[accessor4.bufferView];
							const tinygltf::Buffer& buffer4 = model.buffers[bufferView4.buffer];
							const std::string& bufferData4 = std::string(std::string::size_type(buffer4.data.data()), buffer4.data.size());
							const std::string& texCoordsData = std::string(bufferData4.data() + bufferView4.byteOffset + accessor4.byteOffset, accessor4.count * accessor4.ByteStride(bufferView4));
							TN_CORE_INFO("{0}", texCoordsData);
						}
					}
				}
			}
		}

		TN_CORE_INFO("Done Loading {0}", filepath);
	}
}
