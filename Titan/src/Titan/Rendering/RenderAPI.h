#pragma once
enum class API
{
	Vulkan,
	DX12,
};

namespace Titan
{
	class RenderAPI
	{
	public:
		static void Initialize(const API& api);
	private:
		static void InitializeVulkan();

	 	inline static API s_CurrentAPI;
	};
}
