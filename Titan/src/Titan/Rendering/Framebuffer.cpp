#include "TNpch.h"
#include "Framebuffer.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <dx12helpers/d3dx12.h>
namespace Titan
{
	
	Framebuffer::Framebuffer(const FramebufferInfo& info)
	{
		m_FBInfo = info;
		
	}
	void Framebuffer::Resize(const size_t width, const size_t height)
	{
	}
	void Framebuffer::Clear(const glm::vec4& color)
	{
		
	}
	void Framebuffer::Bind()
	{
		
	}
	void Framebuffer::Unbind()
	{
	}
	Ref<Framebuffer> Titan::Framebuffer::Create(const FramebufferInfo& info)
	{
		return CreateRef<Framebuffer>(info);
	}
	void Framebuffer::Validate()
	{
		
	}
}