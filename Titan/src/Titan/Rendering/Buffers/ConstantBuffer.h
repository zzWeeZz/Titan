#pragma once
#include <d3d12.h>
#include "Titan/Rendering/GraphicsContext.h"
#include "Titan/Core/Core.h"
namespace Titan
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer(size_t sizeOfObject);
		void SetData(void* data, size_t sizeOfData, size_t drawID);
		void Bind(uint32_t drawID, size_t sizeOfData);
		static Ref<ConstantBuffer> Create(size_t sizeOfObject);
		~ConstantBuffer();
	private:
		std::array<WinRef<ID3D12Resource>, g_FrameCount> m_CBufferUploadHeap;
		uint8_t* m_ConstantBufferGPUAddress[g_FrameCount];
	};
}
