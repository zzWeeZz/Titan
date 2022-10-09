#pragma once
namespace Titan
{
	struct FramebufferInfo
	{
		size_t width = 1280u;
		size_t height = 720u;
		//Imageformat vector

	};
	class Framebuffer
	{
	public:
		Framebuffer(const FramebufferInfo& info);
		void Resize(const size_t width, const size_t height);
		void Bind();
		void Unbind();
		static Ref<Framebuffer> Create(const FramebufferInfo& info);
	private:
		void Validate();
	};
}