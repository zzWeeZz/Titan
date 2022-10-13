#pragma once
#include "Titan/Core/TitanFormats.h"
#include "GraphicsContext.h"
#include "glm/glm.hpp"
namespace Titan
{
	struct FramebufferInfo
	{
		size_t width = 1280u;
		size_t height = 720u;
		std::vector<ImageFormat> imageFormats;
	};
	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferInfo& info);
		void Resize(const size_t width, const size_t height);
		void Clear(const glm::vec4& color = { 0.32f, 0.32f, 0.32f, 1.f });
		WinRef<ID3D12Resource> GetCurrentRtv();
		void Bind();
		void Unbind();
		static Ref<Framebuffer> Create(const FramebufferInfo& info);
	private:
		void Validate();
		WinRef<ID3D12DescriptorHeap> m_RtvDescriptorHeap;
		std::array<std::vector<WinRef<ID3D12Resource>>, g_FrameCount> m_RenderTargets;
		WinRef<ID3D12DescriptorHeap> m_DsvDescriptorHeap;
		std::array<WinRef<ID3D12Resource>, g_FrameCount> m_DepthTarget;

		int32_t m_RtvDescriptorSize;
		int32_t m_DvsDescriptorSize;
		FramebufferInfo m_FBInfo;
	};
}