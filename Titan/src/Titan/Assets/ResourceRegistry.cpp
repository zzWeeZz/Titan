#include "TNpch.h"
#include "ResourceRegistry.h"

#include "yaml-cpp/yaml.h"
#include "Titan/Assets/Model/ModelHandle.h"
const std::string filePath = "Engine/Resources/ResourceRegistry.yaml";
void Titan::ResourceRegistry::Initialize()
{
	Load();
}

void Titan::ResourceRegistry::Dump()
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "ResourceRegistry" << YAML::Value;
	out << YAML::BeginSeq;
	size_t index = 0;
	for (auto& it : s_Registry)
	{
		out << YAML::BeginMap;
		if (it.second)
		{
			out << YAML::Key << index << YAML::BeginSeq << it.first << it.second->GetPath().string() << YAML::EndSeq;
		}
		else
		{
			out << YAML::Key << index << YAML::BeginSeq << it.first << "NULL" << YAML::EndSeq;
		}
		out << YAML::EndMap;
		index++;
	}
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filePath);
	fout << out.c_str();
}

void Titan::ResourceRegistry::Load()
{
	std::ifstream fin(filePath);
	std::stringstream buffer;
	buffer << fin.rdbuf();
	YAML::Node node = YAML::Load(buffer.str());

	auto regs = node["ResourceRegistry"].as < std::vector <YAML::Node>>();
	size_t index = 0;
	for (auto& reg : regs)
	{
		auto data = reg[index].as<std::vector<std::string>>();

		s_CachedRegistry[data[0]] = data[1];

		index++;
	}
}
