#pragma once
namespace Titan
{
	class GraphicsContext
	{
	public:
		static void Initialize();
	private:
		static void InitializeSwapChain();
		static void InitializeRasterizer();
	};
}
