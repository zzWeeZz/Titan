#pragma once
#include <filesystem>
namespace Titan
{
	class Resource
	{
	public:
		Resource(){}
		virtual void Initialize(const std::filesystem::path& path) { SetPath(path); };
		std::filesystem::path GetPath() const { return m_Path; }
		virtual ~Resource(){}
	protected:
		void SetPath(const std::filesystem::path& path) { m_Path = path; }
		std::filesystem::path m_Path;
	};
}