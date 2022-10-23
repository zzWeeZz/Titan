#include "TNpch.h"
#include "Renderer.h"

#include "Titan/Application.h"

#include <glm/gtx/transform.hpp>

#include "Titan/Rendering/GraphicsContext.h"
#include "Titan/Rendering/Pipeline/Pipeline.h"
#include <Titan/Rendering/Buffers/VertexBuffer.h>
#include <Titan/Rendering/Buffers/IndexBuffer.h>
#include <Titan/Rendering/Vertices.h>
#include <Titan/Rendering/Framebuffer.h>
#include <dx12helpers/d3dx12.h>
#include <Titan/Rendering/Buffers/ConstantBuffer.h>
#include "Titan/Rendering/Buffers/ConstantBuffers.h"
#include <Optick/src/optick.h>
namespace Titan
{
	struct Cache
	{
		CameraCmd currentCamera = {};
		std::vector<MeshCmd> meshCmds;

		Ref<Framebuffer> testFB;
		Ref<Pipeline> pipeline;
		CameraData cameraData = {};
		Ref<ConstantBuffer> cameraBuffer;

		ModelData modelData = {};
		Ref<ConstantBuffer> modelBuffer;

	};
	static Scope<Cache> s_Cache = CreateScope<Cache>();
	void Renderer::Submit(const CameraCmd& cameraCmd)
	{
		s_Cache->currentCamera = cameraCmd;
	}
	void Renderer::Submit(const MeshCmd& meshCmd)
	{
	}
	void Renderer::Initialize()
	{
		PipelineInfo info{};
		info.vsPath = "Engine/Shaders/triangle_vs.spv";
		info.psPath = "Engine/Shaders/triangle_fs.spv";
		s_Cache->pipeline = Pipeline::Create(info);
	}

	void Renderer::Begin()
	{
		auto& currentFrame = GraphicsContext::GetCurrentFrame();
		auto& commandBuffer = GraphicsContext::GetDevice().GetCommandBuffer(currentFrame, 0);
		auto& swapchain = GraphicsContext::GetSwapchain();
		auto& device = GraphicsContext::GetDevice();
		auto& handle = s_Cache->pipeline->GetHandle();

		vkWaitForFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame), VK_TRUE, UINT64_MAX);
		vkResetFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame));
		uint32_t imageIndex;
		vkAcquireNextImageKHR(device.GetHandle(), swapchain.GetHandle(), UINT64_MAX, swapchain.GetImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE, &imageIndex);

		vkResetCommandBuffer(commandBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional
		TN_VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

		{
			const VkImageMemoryBarrier image_memory_barrier
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.image = swapchain.GetImage(imageIndex),
				.subresourceRange =
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				}
			};

			vkCmdPipelineBarrier(
				commandBuffer,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
				0,
				0,
				nullptr,
				0,
				nullptr,
				1, // imageMemoryBarrierCount
				&image_memory_barrier // pImageMemoryBarriers
			);
		}

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };

		const VkRenderingAttachmentInfoKHR colorAttachmentInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
			.imageView = swapchain.GetCurrentView(imageIndex),
			.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = clearColor,
		};
		VkRect2D rec;
		rec.extent = swapchain.GetExtent();
		rec.offset = { 0,0 };
		const VkRenderingInfoKHR render_info{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
			.renderArea = rec,
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentInfo,
		};
		auto BeginRendering = (PFN_vkCmdBeginRenderingKHR)vkGetInstanceProcAddr(GraphicsContext::GetInstance(), "vkCmdBeginRenderingKHR");
		BeginRendering(commandBuffer, &render_info);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(Application::GetWindow().GetWidth());
		viewport.height = static_cast<float>(Application::GetWindow().GetHeight());
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchain.GetExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		auto EndRendering = (PFN_vkCmdEndRenderingKHR)vkGetInstanceProcAddr(GraphicsContext::GetInstance(), "vkCmdEndRenderingKHR");
		EndRendering(commandBuffer);
		{
			VkImageMemoryBarrier image_memory_barrier
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				.image = swapchain.GetImage(imageIndex),
				.subresourceRange =
				{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1,
				}
			};


			vkCmdPipelineBarrier(
				commandBuffer,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // srcStageMask
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // dstStageMask
				0,
				0,
				nullptr,
				0,
				nullptr,
				1, // imageMemoryBarrierCount
				&image_memory_barrier // pImageMemoryBarriers
			);
		}

		TN_VK_CHECK(vkEndCommandBuffer(commandBuffer));

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { swapchain.GetImageAvailableSemaphore(currentFrame) };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphores[] = { swapchain.GetRenderFinishedSemaphore(currentFrame) };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		TN_VK_CHECK(vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, swapchain.GetInFlight(currentFrame)));

		vkWaitForFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame), VK_TRUE, UINT64_MAX);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapchain.GetHandle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
		currentFrame = (currentFrame + 1) % g_FramesInFlight;
	}

	void Renderer::Shutdown()
	{
	}
}
