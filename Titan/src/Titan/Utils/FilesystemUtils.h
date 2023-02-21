#pragma once
namespace Titan
{
	class FilesystemUtils
	{
	public:
		inline static const std::filesystem::path TempEntry = "Engine\\Temp\\";
		static void ReadBinary(const std::filesystem::path& filepath, std::vector<char>& outBinaryData);
		static void WriteBinary(const std::filesystem::path& filepath, std::vector<char>& inBinaryData);
	};
}