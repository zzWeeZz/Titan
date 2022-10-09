#pragma once
#include <d3d12.h>
#include "Titan/Core/TitanMemory.h"
namespace Titan
{
	struct PipelineInfo
	{
		std::filesystem::path vsPath;
		std::filesystem::path psPath;

	};
	class Pipeline
	{
		friend class GraphicsContext;
	public:
		Pipeline(const PipelineInfo& info);
		void Bind();
		static Ref<Pipeline> Create(const PipelineInfo& info);
	private:
		void GetInputLayoutFromBlob(ID3DBlob* blob, std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout);
		WinRef<ID3D12PipelineState> m_PipelineStateObject; // pso containing a pipeline state

		WinRef<ID3D12RootSignature> m_RootSignature; // root signature defines data shaders will access
	};
}
