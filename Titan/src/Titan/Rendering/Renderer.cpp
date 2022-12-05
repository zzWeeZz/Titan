#include "TNpch.h"
#include "Renderer.h"

#include <imgui.h>
#include <Optick/src/optick.h>
#include <glm/gtx/transform.hpp>

#include "Titan/Application.h"

#include "Titan/ImGui/TitanImGui.h"

#include "Titan/Assets/Texture/Texture.h"
#include "Titan/Assets/ResourceRegistry.h"

#include "Titan/Rendering/Vertices.h"
#include "Titan/Rendering/Framebuffer.h"
#include "Titan/Rendering/GraphicsContext.h"

#include <Titan/Rendering/Buffers/IndexBuffer.h>
#include <Titan/Rendering/Buffers/VertexBuffer.h>
#include <Titan/Rendering/Buffers/UniformBuffer.h>
#include "Titan/Rendering/Buffers/UniformBuffers.h"

#include "Titan/Rendering/Libraries/SamplerLibrary.h"
#include "Titan/Rendering/Libraries/PipelineLibrary.h"

#include "Titan/Rendering/Descriptors/DescriptorBuilder.h"
#include "Titan/Rendering/Descriptors/DescriptorAllocator.h"
#include "Titan/Rendering/Descriptors/DescriptorLayoutCache.h"

namespace Titan
{
	struct Cache
	{
		CameraCmd currentCamera = {};
		LightCmd lightData = {};
		std::vector<MeshCmd> meshCmds;

		Ref<Framebuffer> mainFB;
		Ref<VertexBuffer> vbtest;
		Ref<IndexBuffer> ibtest;
		Ref<Framebuffer> testFB;
		CameraData cameraData = {};
		Ref<UniformBuffer> cameraBuffer;
		TitanID textureID;
		ModelData modelData = {};
		Ref<UniformBuffer> lightBuffer;

		DescriptorAllocator allocator;
		DescriptorLayoutCache cache;
	};
	static Scope<Cache> s_Cache = CreateScope<Cache>();
	static PerFrameInFlight<VkDescriptorSet> s_DescriptorSets;
	static std::vector<PerFrameInFlight<VkDescriptorSet>> s_ExternalDescriptorSets;
	void Renderer::Submit(const CameraCmd& cameraCmd)
	{
		s_Cache->currentCamera = cameraCmd;
	}
	void Renderer::Submit(const MeshCmd& meshCmd)
	{
		s_Cache->meshCmds.push_back(meshCmd);
	}

	void Renderer::Submit(const LightCmd& lightCmd)
	{
		s_Cache->lightData = lightCmd;
	}

	Ref<Framebuffer> Renderer::GetMainFramebuffer()
	{
		return s_Cache->mainFB;
	}

	void Renderer::Initialize()
	{
		ResourceRegistry::GetItem<Texture>(s_Cache->textureID)->Initialize("Assets/Texture/Titan.png");

		GraphicsPipelineInfo info{};
		info.vsPath = "Engine/Shaders/triangle_vs.vert";
		info.psPath = "Engine/Shaders/triangle_fs.frag";

		info.topology = Topology::TriangleList;
		info.imageFormats = { ImageFormat::R8G8B8A8_UN, ImageFormat::D32_SF_S8_UI };
		PipelineLibrary::Add("Mesh", info);

		s_Cache->cameraBuffer = UniformBuffer::Create({ &s_Cache->cameraData, sizeof(CameraData) });
		s_Cache->lightBuffer = UniformBuffer::Create({ &s_Cache->lightData, sizeof(LightCmd) });

		FramebufferInfo fbInfo{};
		fbInfo.width = Application::GetWindow().GetWidth();
		fbInfo.height = Application::GetWindow().GetHeight();
		fbInfo.imageFormats = { ImageFormat::R8G8B8A8_UN, ImageFormat::D32_SF_S8_UI };
		s_Cache->mainFB = Framebuffer::Create(fbInfo);

		SamplerLibrary::Add("Clamp", Filter::Linear, Address::ClampToEdge, MipmapMode::Linear);
	}

	void Renderer::Begin()
	{
		TN_PROFILE_FUNCTION();
		auto& currentFrame = GraphicsContext::GetCurrentFrame();
		auto& commandBuffer = GraphicsContext::GetDevice().GetCommandBuffer(currentFrame, 0);
		auto& swapchain = GraphicsContext::GetSwapchain();
		auto& device = GraphicsContext::GetDevice();
		auto& handle = PipelineLibrary::Get("Mesh")->GetHandle();

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = s_Cache->cameraBuffer->GetAllocation().buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(CameraData);

		vkWaitForFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame), VK_TRUE, UINT64_MAX);
		auto index = GraphicsContext::GetSwapchain().GetNextImage();
		if (index < 0)
		{
			ImGui::EndFrame();
			return;
		}
		vkResetFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame));
		uint32_t imageIndex = index;
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
				.image = s_Cache->mainFB->GetImages()[0].Image,
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
		VkClearValue depthClear{};
		depthClear.depthStencil.depth = 1.f;
		depthClear.depthStencil.stencil = 0;

		const VkRenderingAttachmentInfo colorAttachmentInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = s_Cache->mainFB->GetViews()[0],
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = clearColor,
		};
		const VkRenderingAttachmentInfo depthAttachmentInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = s_Cache->mainFB->GetViews()[1],
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = depthClear,
		};
		VkRect2D rec{};
		rec.extent.width = static_cast<uint32_t>(s_Cache->mainFB->GetInfo().width);
		rec.extent.height = static_cast<uint32_t>(s_Cache->mainFB->GetInfo().height);
		rec.offset = { 0,0 };
		const VkRenderingInfo render_info{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = rec,
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentInfo,
			.pDepthAttachment = &depthAttachmentInfo,
		};

		{
			TN_PROFILE_CONTEXT(commandBuffer);
			TN_PROFILE_SCOPE("Forward pass");
			vkCmdBeginRendering(commandBuffer, &render_info);

			PipelineLibrary::BindPipline("Mesh", commandBuffer);
			s_Cache->mainFB->Bind(commandBuffer);



			s_Cache->cameraData.proj = s_Cache->currentCamera.proj;
			s_Cache->cameraData.view = s_Cache->currentCamera.view;
			s_Cache->lightBuffer->SetData(&s_Cache->lightData, sizeof(LightCmd));
			s_Cache->cameraBuffer->SetData(&s_Cache->cameraData, sizeof(CameraData));
			TN_PROFILE_CONTEXT(commandBuffer);
			TN_PROFILE_SCOPE("Draw scene");
			for (auto& mdlCmd : s_Cache->meshCmds)
			{
				auto texture = ResourceRegistry::GetItem<Texture>(mdlCmd.textureId);
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = texture->GetView();
				imageInfo.sampler = texture->GetSampler();

				VkDescriptorBufferInfo lightBufferInfo{};
				lightBufferInfo.buffer = s_Cache->lightBuffer->GetAllocation().buffer;
				lightBufferInfo.offset = 0;
				lightBufferInfo.range = sizeof(LightCmd);
				VkDescriptorSet globalSet;
				DescriptorBuilder::Begin(&s_Cache->cache, &s_Cache->allocator)
					.BindBuffer(0, &bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
					.BindImage(1, &imageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
					.BindBuffer(2, &lightBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
					.Build(globalSet);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLibrary::Get("Mesh")->GetLayout(), 0, 1, &globalSet, 0, nullptr);

				VkDeviceSize offset = 0;
				auto& vertex = mdlCmd.package.vertexBuffer;
				auto& index = mdlCmd.package.indexBuffer;


				vkCmdPushConstants(commandBuffer, PipelineLibrary::Get("Mesh")->GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mdlCmd.transform);

				vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertex->GetAllocation().buffer, &offset);
				vkCmdBindIndexBuffer(commandBuffer, index->GetAllocatedBuffer().buffer, 0, VK_INDEX_TYPE_UINT32);
				vkCmdDrawIndexed(commandBuffer, index->GetIndexCount(), 1, 0, 0, 0);
			}

			vkCmdEndRendering(commandBuffer);
		}


		TitanImGui::End();
		TN_VK_CHECK(vkEndCommandBuffer(commandBuffer));


		VkSemaphore signalSemaphores[] = { swapchain.GetRenderFinishedSemaphore(currentFrame) };

		{
			TN_PROFILE_SCOPE("Queue submit");
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores[] = { swapchain.GetImageAvailableSemaphore(currentFrame) };
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			TN_VK_CHECK(vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, swapchain.GetInFlight(currentFrame)));
		}
		{
			TN_PROFILE_SCOPE("swapchain present");
			vkWaitForFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame), VK_TRUE, UINT64_MAX);

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;

			VkSwapchainKHR swapChains[] = { swapchain.GetHandle() };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &imageIndex;
			OPTICK_GPU_FLIP(&swapchain.GetHandle());
			vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
		}

		currentFrame = (currentFrame + 1) % g_FramesInFlight;
		TitanImGui::FlushDescriptors();
		s_Cache->allocator.ResetPools();
		s_Cache->meshCmds.clear();
	}


	void Renderer::BeginPass(const std::string& passName)
	{

	}

	void Renderer::EndPass()
	{
	}

	void Renderer::Shutdown()
	{
		s_Cache->allocator.Shutdown();
		s_Cache->cache.Shutdown();
		s_Cache->mainFB->CleanUp();
	}

}
