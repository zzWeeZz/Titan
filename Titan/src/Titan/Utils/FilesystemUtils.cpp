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
}