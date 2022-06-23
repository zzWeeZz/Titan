#pragma once
#include <filesystem>
#include <fstream>
#define USE_SERIALIZER
#include <numeric>
#include <random>

#include "Snowflake.hpp"
namespace Snowflake
{

	class RegistrySerializer
	{
	public:
		RegistrySerializer(Registry& registry);
		bool Serialize(const std::filesystem::path& filePath);
		bool Deserialize(const std::filesystem::path& filePath);
	private:
		Registry& m_Registry;
	};

	inline RegistrySerializer::RegistrySerializer(Registry& registry) : m_Registry(registry)
	{
	}

	inline bool RegistrySerializer::Serialize(const std::filesystem::path& filePath)
	{
		std::ofstream writeFile(filePath.string(), std::ios::out | std::ios::binary);
		if (!writeFile)
		{
			return false;
		}
		uint32_t entityByteLength = 0;
		std::vector<SnowID> currentEntityComponents;
		auto data = m_Registry.m_Entities.size();
		writeFile.write(reinterpret_cast<char*>(&data), sizeof(size_t));
		m_Registry.ForEach([&](auto entity)
			{
				for (auto it : m_Registry.m_ComponentPools)
				{
					if (it.second.IsEntityRegistered(entity))
					{
						currentEntityComponents.push_back(it.first);
						entityByteLength += componentSizes[it.first];
					}
				}
				writeFile.write(reinterpret_cast<char*>(&entityByteLength), sizeof(uint32_t));
				for (auto it : currentEntityComponents)
				{
					writeFile.write(reinterpret_cast<char*>(&componentSizes[it]), sizeof(size_t));
					auto pCompData = m_Registry.m_ComponentPools[it].GetComponentData(entity);
					
					writeFile.write(reinterpret_cast<char*>(&pCompData[0]), componentSizes[it]);
				}
				entityByteLength = 0;
				currentEntityComponents.clear();
			});
		writeFile.close();
		if (!writeFile.good())
		{
			return false;
		}
		return true;
	}

	inline bool RegistrySerializer::Deserialize(const std::filesystem::path& filePath)
	{
		std::ifstream readFile(filePath.string(), std::ios::out | std::ios::binary);
		if (!readFile)
		{
			return false;
		}
		size_t entityCount = 0;
		readFile.read(reinterpret_cast<char*>(&entityCount), sizeof(size_t));
		for (size_t i = 0; i < entityCount; ++i)
		{
			uint32_t entityByteLength = 0;
			readFile.read(reinterpret_cast<char*>(&entityByteLength), sizeof(uint32_t));
			auto entity = m_Registry.CreateEntity();
			while (entityByteLength > 0)
			{
				size_t componentLength = 0;
				readFile.read(reinterpret_cast<char*>(&componentLength), sizeof(size_t));
				std::vector<uint8_t> componentData;
				componentData.resize(componentLength);
				readFile.read(reinterpret_cast<char*>(&componentData[0]), componentLength);
				SnowID readID;
				memcpy(&readID, componentData.data(), sizeof(SnowID));
				m_Registry.AddComponentFromData(componentData, readID, entity);
				entityByteLength -= componentLength;

			}
		}
		readFile.close();
		if (!readFile.good())
		{
			return false;
		}
		return true;
	}
}
