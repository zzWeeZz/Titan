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
		static void Begin();
		static void Shutdown();
	private:
		static void InitializeVulkan();
		static void BeginVulkanRender();
		static void ShutdownVulkan();

	 	inline static API s_CurrentAPI;
	};
}
