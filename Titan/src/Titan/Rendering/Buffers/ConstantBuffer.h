#pragma once
#include <d3d12.h>
#include "Titan/Rendering/GraphicsContext.h"
namespace Titan
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer();
		void SetData(void* data, size_t sizeOfData);
		void Bind(const uint32_t bindPoint);
		static Ref<ConstantBuffer> Create();
	private:
		std::array<WinRef<ID3D12DescriptorHeap>, g_FrameCount> m_MainDescriptorHeap;
		std::array<WinRef<ID3D12Resource>, g_FrameCount> m_CBufferUploadHeap;
		uint8_t* m_ConstantBufferGPUAddress[g_FrameCount];
	};
}
