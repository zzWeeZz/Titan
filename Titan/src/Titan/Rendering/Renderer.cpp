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
#include <Titan/Rendering/Buffers/UniformBuffer.h>
#include "Titan/Rendering/Buffers/UniformBuffers.h"
#include <Optick/src/optick.h>
#include "Titan/Assets/ResourceRegistry.h"
#include <Titan/Assets/Texture/Texture.h>
#include "Titan/ImGui/TitanImGui.h"
namespace Titan
{
	struct Cache
	{
		CameraCmd currentCamera = {};
		std::vector<MeshCmd> meshCmds;

		Ref<Framebuffer> mainFB;
		Ref<VertexBuffer> vbtest;
		Ref<IndexBuffer> ibtest;
		Ref<Framebuffer> testFB;
		Ref<Pipeline> pipeline;
		CameraData cameraData = {};
		Ref<UniformBuffer> cameraBuffer;
		TitanID textureID;
		ModelData modelData = {};
		Ref<UniformBuffer> modelBuffer;

		VkDescriptorPool descriptorPool;
	};
	static Scope<Cache> s_Cache = CreateScope<Cache>();
	static PerFrameInFlight<VkDescriptorSet> descriptorSets;
	void Renderer::Submit(const CameraCmd& cameraCmd)
	{
		s_Cache->currentCamera = cameraCmd;
	}
	void Renderer::Submit(const MeshCmd& meshCmd)
	{
		s_Cache->meshCmds.push_back(meshCmd);
	}
	void Renderer::Initialize()
	{
		std::array<VkDescriptorPoolSize, 2> poolSize{};
		poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize[0].descriptorCount = g_FramesInFlight;
		poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize[1].descriptorCount = g_FramesInFlight;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = poolSize.size();
		poolInfo.pPoolSizes = poolSize.data();

		poolInfo.maxSets = g_FramesInFlight;

		TN_VK_CHECK(vkCreateDescriptorPool(GraphicsContext::GetDevice().GetHandle(), &poolInfo, nullptr, &s_Cache->descriptorPool));
		TitanAllocator::QueueDeletion([&]() {vkDestroyDescriptorPool(GraphicsContext::GetDevice().GetHandle(), s_Cache->descriptorPool, nullptr); });
		
		ResourceRegistry::GetItem<Texture>(s_Cache->textureID)->Initialize("Assets/Texture/Titan.png");

		PipelineInfo info{};
		info.vsPath = "Engine/Shaders/triangle_vs.spv";
		info.psPath = "Engine/Shaders/triangle_fs.spv";
		s_Cache->pipeline = Pipeline::Create(info);

		s_Cache->cameraBuffer = UniformBuffer::Create({ &s_Cache->cameraData, sizeof(CameraData) });

		std::vector<VkDescriptorSetLayout> layouts(g_FramesInFlight, s_Cache->pipeline->DescLayout());
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = s_Cache->descriptorPool;
		allocInfo.descriptorSetCount = (g_FramesInFlight);
		allocInfo.pSetLayouts = layouts.data();

		FramebufferInfo fbInfo{};
		fbInfo.width = Application::GetWindow().GetWidth();
		fbInfo.height = Application::GetWindow().GetHeight();
		fbInfo.imageFormats = { ImageFormat::R8G8B8A8_SRGB };
		s_Cache->mainFB = Framebuffer::Create(fbInfo);

		TN_VK_CHECK(vkAllocateDescriptorSets(GraphicsContext::GetDevice().GetHandle(), &allocInfo, descriptorSets.data()));
	}

	void Renderer::Begin()
	{
		auto& currentFrame = GraphicsContext::GetCurrentFrame();
		auto& commandBuffer = GraphicsContext::GetDevice().GetCommandBuffer(currentFrame, 0);
		auto& swapchain = GraphicsContext::GetSwapchain();
		auto& device = GraphicsContext::GetDevice();
		auto& handle = s_Cache->pipeline->GetHandle();

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = s_Cache->cameraBuffer->GetAllocation().buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(CameraData);

		auto texture = ResourceRegistry::GetItem<Texture>(s_Cache->textureID);
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture->GetView();
		imageInfo.sampler = texture->GetSampler();

		std::array<VkWriteDescriptorSet, 2> descriptorWrite{};
		descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite[0].dstBinding = 0;
		descriptorWrite[0].descriptorCount = 1;
		descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite[0].pBufferInfo = &bufferInfo;
		descriptorWrite[0].dstSet = descriptorSets[GraphicsContext::GetCurrentFrame()];
		
		descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite[1].dstBinding = 1;
		descriptorWrite[1].descriptorCount = 1;
		descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite[1].dstSet = descriptorSets[GraphicsContext::GetCurrentFrame()];
		descriptorWrite[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(GraphicsContext::GetDevice().GetHandle(), descriptorWrite.size(), descriptorWrite.data(), 0, nullptr);


		vkWaitForFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame), VK_TRUE, UINT64_MAX);
		vkResetFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame));
		auto imageIndex = GraphicsContext::GetSwapchain().GetNextImage();

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

		const VkRenderingAttachmentInfo colorAttachmentInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = s_Cache->mainFB->GetViews()[0],
			.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = clearColor,
		};
		VkRect2D rec;
		rec.extent = swapchain.GetExtent();
		rec.offset = { 0,0 };
		const VkRenderingInfo render_info{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = rec,
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentInfo,
		};
		vkCmdBeginRendering(commandBuffer, &render_info);

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
		

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_Cache->pipeline->GetLayout(), 0, 1, &descriptorSets[GraphicsContext::GetCurrentFrame()], 0, nullptr);

		s_Cache->cameraData.proj = s_Cache->currentCamera.proj;
		s_Cache->cameraData.view = s_Cache->currentCamera.view;
		s_Cache->cameraData.mdlSpace = glm::mat4(1.f);

		s_Cache->cameraBuffer->SetData(&s_Cache->cameraData, sizeof(CameraData));

		for (auto& mdlCmd : s_Cache->meshCmds)
		{
			VkDeviceSize offset = 0;
			auto& vertex = mdlCmd.package.vertexBuffer;
			auto& index = mdlCmd.package.indexBuffer;

			vkCmdPushConstants(commandBuffer, s_Cache->pipeline->GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mdlCmd.transform);

			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertex->GetAllocation().buffer, &offset);
			vkCmdBindIndexBuffer(commandBuffer, index->GetAllocatedBuffer().buffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(commandBuffer, index->GetIndexCount(), 1, 0, 0, 0);
		}

		vkCmdEndRendering(commandBuffer);
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

		TitanImGui::End();

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

		s_Cache->meshCmds.clear();
	}

	void Renderer::Shutdown()
	{
	}
}
