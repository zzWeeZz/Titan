#pragma once



namespace Titan
{
	class Renderer
	{
	public:
		static void Initialize();
		static void Begin();
		static void End();
		// this will draw command buffers; DO NOT USE THIS TO RENDER SOMETHING, This is called internal. 
		static void DrawCommands();
		static void Shutdown();
	private:
		static void InitializePipelines();
	};
}
