#pragma once
namespace Titan
{
	class FilesystemUtils
	{
	public:
		static void ReadBinary(const std::filesystem::path& filepath, std::vector<char>& outBinaryData);
	};
}