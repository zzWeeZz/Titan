#pragma once
#include <d3d12.h>
#include "Titan/Core/TitanMemory.h"
namespace Titan
{
	struct IndexBufferInfo
	{
		DWORD* indexData;
		size_t sizeOfArray;
		std::wstring debugName;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(const IndexBufferInfo& info);
		void Bind();
		static Ref<IndexBuffer> Create(const IndexBufferInfo& info);
	private:
		WinRef<ID3D12Resource> m_IndexBuffer; // a default buffer in GPU memory that we will load vertex data for our triangle into

		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
	};
}