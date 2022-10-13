#pragma once

namespace Titan
{
	class Resource
	{
	public:
		Resource(){}
		virtual void Initialize(const std::filesystem::path& path) = 0;
		std::filesystem::path GetPath() const { return m_Path; }
		virtual ~Resource(){}
	protected:
		void SetPath(const std::filesystem::path& path) { m_Path = path; }
		std::filesystem::path m_Path;
	};
}