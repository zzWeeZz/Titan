#pragma once
namespace Titan
{



	class FilesystemUtils
	{
	public:
		inline static const std::filesystem::path TempEntry = "Engine/Temp/";
		/*template<typename T>
		static void ReadFileData(const std::filesystem::path& filepath, std::vector<T>& outData);*/
		static void ReadBinary(const std::filesystem::path& filepath, std::vector<char>& outBinaryData);
		
		/*template<typename T>
		static void WriteFileData(const std::filesystem::path& filepath, std::vector<T>& inData);*/
		static void WriteBinary(const std::filesystem::path& filepath, std::vector<char>& inBinaryData);
	};
	/*template<typename T>
	inline void FilesystemUtils::ReadFileData(const std::filesystem::path& filepath, std::vector<T>& outData)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::in);

		TN_CORE_ASSERT(file.is_open(), "Could not open file: {0}", filepath.string());
		size_t fileSize = static_cast<size_t>(file.tellg());
		outBinaryData.resize(fileSize);
		file.seekg(0);
		file.read(outBinaryData.data(), fileSize);
		file.close();
	}
	template<typename T>
	inline void FilesystemUtils::WriteFileData(const std::filesystem::path& filepath, std::vector<T>& inData)
	{
	}*/
}