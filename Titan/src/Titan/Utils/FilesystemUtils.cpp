#include "TNpch.h"
#include "FilesystemUtils.h"
namespace Titan
{

	void FilesystemUtils::ReadBinary(const std::filesystem::path& filepath, std::vector<char>& outBinaryData)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		TN_CORE_ASSERT(file.is_open(), "Could not open file: {0}", filepath.string());
		size_t fileSize = static_cast<size_t>(file.tellg());
		outBinaryData.resize(fileSize);
		file.seekg(0);
		file.read(outBinaryData.data(), fileSize);
		file.close();
	}
	void FilesystemUtils::WriteBinary(const std::filesystem::path& filepath, std::vector<char>& inBinaryData)
	{
		if (!std::filesystem::is_directory(filepath) || !std::filesystem::exists(filepath))
		{
			std::filesystem::create_directories(filepath.parent_path());
		}
		std::ofstream file(filepath, std::ios::binary);
		TN_CORE_ASSERT(!file.bad(), "Failed to create file: {0}", filepath.string());
		file.write(inBinaryData.data(), inBinaryData.size() * sizeof(char));
		file.close();
	}
}