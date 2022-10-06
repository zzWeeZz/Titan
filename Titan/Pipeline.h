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
	public:
		Pipeline(const PipelineInfo& info);
		static Ref<Pipeline> Create(const PipelineInfo& info);
	private:
		void GetInputLayoutFromBlob(ID3DBlob* blob, D3D12_INPUT_LAYOUT_DESC& InputLayout);
		WinRef<ID3D12PipelineState> m_PipelineStateObject; // pso containing a pipeline state

		WinRef<ID3D12RootSignature> m_RootSignature; // root signature defines data shaders will access

		D3D12_VIEWPORT m_Viewport; // area that output from rasterizer will be stretched to.

		D3D12_RECT m_ScissorRect; // the area to draw in. pixels outside that area will not be drawn onto

		WinRef<ID3D12Resource> m_VertexBuffer; // a default buffer in GPU memory that we will load vertex data for our triangle into

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView; // a structure containing a pointer to the vertex data in gpu memory
												   // the total size of the buffer, and the size of each element (vertex)
	};
}
