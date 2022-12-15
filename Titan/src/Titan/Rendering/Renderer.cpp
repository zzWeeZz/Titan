#include "TNpch.h"
#include "Renderer.h"

#include <imgui.h>
#include <Optick/src/optick.h>
#include <glm/gtx/transform.hpp>

#include "Titan/Application.h"

#include "Titan/ImGui/TitanImGui.h"

#include "Titan/Utils/MemoryVector.h"

#include "Titan/Assets/Model/Submesh.h"
#include "Titan/Assets/Texture/Texture.h"
#include "Titan/Assets/ResourceRegistry.h"

#include "Titan/Rendering/Vertices.h"
#include "Titan/Rendering/Framebuffer.h"
#include "Titan/Rendering/BindRegistry.h"
#include "Titan/Rendering/GraphicsContext.h"

#include <Titan/Rendering/Buffers/IndexBuffer.h>
#include <Titan/Rendering/Buffers/VertexBuffer.h>
#include <Titan/Rendering/Buffers/GenericBuffer.h>
#include <Titan/Rendering/Buffers/StorageBuffer.h>
#include <Titan/Rendering/Buffers/UniformBuffer.h>
#include <Titan/Rendering/Buffers/UniformBuffers.h>

#include "Titan/Rendering/Libraries/SamplerLibrary.h"
#include "Titan/Rendering/Libraries/PipelineLibrary.h"

#include "Titan/Rendering/Descriptors/DescriptorBuilder.h"
#include "Titan/Rendering/Descriptors/DescriptorAllocator.h"
#include "Titan/Rendering/Descriptors/DescriptorLayoutCache.h"

#define MAX_INDIRECT_COMMANDS 10000

namespace Titan
{
	struct Constant
	{
		uint32_t meshletCount;
		uint32_t vertexCount;
		uint32_t indexCount;
		uint32_t padd;
	};
	struct Cache
	{
		CameraCmd currentCamera = {};
		LightCmd lightData = {};
		MemoryVector<MeshCmd> meshCmds;

		Ref<Framebuffer> mainFB;
		Ref<VertexBuffer> vbtest;
		Ref<IndexBuffer> ibtest;
		Ref<Framebuffer> testFB;
		CameraData cameraData = {};
		Ref<UniformBuffer> cameraBuffer;
		TitanID textureID;
		ModelData modelData = {};
		Ref<UniformBuffer> lightBuffer;
		Constant constant;
		PerFrameInFlight<DescriptorAllocator> allocators;
		PerFrameInFlight<DescriptorLayoutCache> caches;
		BindRegistry textureBindSet;
		Ref<GenericBuffer> indirectCmdBuffer;
	};
	static Scope<Cache> s_Cache = CreateScope<Cache>();
	static PerFrameInFlight<VkDescriptorSet> s_DescriptorSets;
	static uint32_t s_RenderDebugState;
	static std::vector<PerFrameInFlight<VkDescriptorSet>> s_ExternalDescriptorSets;

	void Renderer::SetDebugLayer(const uint32_t& layer)
	{
		s_RenderDebugState = layer;
	}

	void Renderer::Submit(const CameraCmd& cameraCmd)
	{
		s_Cache->currentCamera = cameraCmd;
	}
	void Renderer::Submit(const MeshCmd& meshCmd)
	{
		auto& cmd = s_Cache->meshCmds.Emplace();
		cmd.submesh = meshCmd.submesh;
		cmd.textureId = meshCmd.textureId;
		cmd.transform = meshCmd.transform;
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

		info.topology = Topology::TriangleList;
		info.imageFormats = { ImageFormat::R8G8B8A8_UN, ImageFormat::D32_SF_S8_UI };

		info.tsPath = "Engine/Shaders/StaticMesh_ts.glsl";
		info.msPath = "Engine/Shaders/StaticMesh_ms.glsl";
		info.fsPath = "Engine/Shaders/staticMesh_fs.glsl";
		PipelineLibrary::Add("MeshShaders", info);

		s_Cache->cameraBuffer = UniformBuffer::Create({ &s_Cache->cameraData, sizeof(CameraData) });
		s_Cache->lightBuffer = UniformBuffer::Create({ &s_Cache->lightData, sizeof(LightCmd) });

		FramebufferInfo fbInfo{};
		fbInfo.width = Application::GetWindow().GetWidth();
		fbInfo.height = Application::GetWindow().GetHeight();
		fbInfo.imageFormats = { ImageFormat::R8G8B8A8_UN, ImageFormat::D32_SF_S8_UI };
		s_Cache->mainFB = Framebuffer::Create(fbInfo);

		GenericBufferInfo indirectBufferInfo{};
		indirectBufferInfo.data = nullptr;
		indirectBufferInfo.size = MAX_INDIRECT_COMMANDS;
		indirectBufferInfo.stride = sizeof(VkDrawMeshTasksIndirectCommandNV);
		indirectBufferInfo.perFrameInFlight = true;
		indirectBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		indirectBufferInfo.allocUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		s_Cache->indirectCmdBuffer = GenericBuffer::Create(indirectBufferInfo);

		SamplerLibrary::Add("Clamp", Filter::Linear, Address::ClampToEdge, MipmapMode::Linear);
	}

	void Renderer::NewFrame()
	{
		auto& currentFrame = GraphicsContext::GetCurrentFrame();
		auto& swapchain = GraphicsContext::GetSwapchain();
		auto& device = GraphicsContext::GetDevice();
		vkWaitForFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame), VK_TRUE, UINT64_MAX);
		s_Cache->allocators[currentFrame].ResetPools();
		TitanImGui::FlushDescriptors(currentFrame);
	}

	void Renderer::Begin()
	{
		TN_PROFILE_FUNCTION();
		Profiler::PofileDataSet("MeshletCount",size_t(0));
		Profiler::PofileDataSet("TriangleCount",size_t(0));

		auto& currentFrame = GraphicsContext::GetCurrentFrame();
		auto& commandBuffer = GraphicsContext::GetDevice().GetCommandBuffer(currentFrame, 0);
		auto& swapchain = GraphicsContext::GetSwapchain();
		auto& device = GraphicsContext::GetDevice();

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = s_Cache->cameraBuffer->GetAllocation().buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(CameraData);

		auto index = GraphicsContext::GetSwapchain().GetNextImage();

		if (index < 0)
		{
			ImGui::EndFrame();
			return;
		}
		vkResetFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame));
		uint32_t imageIndex = index;
		vkResetCommandBuffer(commandBuffer, 0);

		void* mappedMemory = nullptr;
		TitanAllocator::MapMemory(s_Cache->indirectCmdBuffer->GetAllocation(), mappedMemory);
		VkDrawMeshTasksIndirectCommandNV* drawCommands = (VkDrawMeshTasksIndirectCommandNV*)mappedMemory;
		// Collect indrect buffer cmds
		for (size_t i = 0; i < s_Cache->meshCmds.Size(); ++i)
		{
			drawCommands[i].taskCount = static_cast<uint32_t>(s_Cache->meshCmds[i].submesh->GetMeshlets().size());
			drawCommands[i].firstTask = 0;
		}
		TitanAllocator::UnMapMemory(s_Cache->indirectCmdBuffer->GetAllocation());

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

			PipelineLibrary::BindPipline("MeshShaders", commandBuffer);
			s_Cache->mainFB->Bind(commandBuffer);
			
			s_Cache->cameraData.proj = s_Cache->currentCamera.proj;
			s_Cache->cameraData.view = s_Cache->currentCamera.view;
			s_Cache->lightBuffer->SetData(&s_Cache->lightData, sizeof(LightCmd));
		

			TN_PROFILE_SCOPE("Draw scene");
			for (size_t i = 0; i < s_Cache->meshCmds.Size(); ++i)
			{
				auto& mdlCmd = s_Cache->meshCmds[i];
				auto texture = ResourceRegistry::GetItem<Texture>(mdlCmd.textureId);
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = texture->GetView();
				imageInfo.sampler = texture->GetSampler();

				s_Cache->cameraData.mdlSpace = mdlCmd.transform;
				s_Cache->cameraBuffer->SetData(&s_Cache->cameraData, sizeof(CameraData));

				auto vertex = mdlCmd.submesh->GetVertexBuffer();
				auto triangle = mdlCmd.submesh->GetTriangleBuffer();
				auto meshlet = mdlCmd.submesh->GetMeshletBuffer();
				auto meshletVertex = mdlCmd.submesh->GetMeshletVertexBuffer();
				auto& light = s_Cache->lightBuffer;

				VkDescriptorBufferInfo vbufferInfo{};
				vbufferInfo.buffer = vertex->GetAllocation().buffer;
				vbufferInfo.offset = 0;
				vbufferInfo.range = vertex->GetAllocation().sizeOfBuffer;

				VkDescriptorBufferInfo tbufferInfo{};
				tbufferInfo.buffer = triangle->GetAllocation().buffer;
				tbufferInfo.offset = 0;
				tbufferInfo.range = triangle->GetAllocation().sizeOfBuffer;

				VkDescriptorBufferInfo mbufferInfo{};
				mbufferInfo.buffer = meshlet->GetAllocation().buffer;
				mbufferInfo.offset = 0;
				mbufferInfo.range = meshlet->GetAllocation().sizeOfBuffer;

				VkDescriptorBufferInfo vmbufferInfo{};
				vmbufferInfo.buffer = meshletVertex->GetAllocation().buffer;
				vmbufferInfo.offset = 0;
				vmbufferInfo.range = meshletVertex->GetAllocation().sizeOfBuffer;

				VkDescriptorBufferInfo lightBufferInfo{};
				lightBufferInfo.buffer = light->GetAllocation().buffer;
				lightBufferInfo.offset = 0;
				lightBufferInfo.range = light->GetAllocation().sizeOfBuffer;

				VkDescriptorSet globalSet;
				DescriptorBuilder::Begin(&s_Cache->caches[currentFrame], &s_Cache->allocators[currentFrame])
					.BindBuffer(0, &bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_MESH_BIT_NV)
					.BindBuffer(1, &vbufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_MESH_BIT_NV)
					.BindBuffer(2, &tbufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_MESH_BIT_NV)
					.BindBuffer(3, &mbufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_MESH_BIT_NV)
					.BindBuffer(4, &vmbufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_MESH_BIT_NV)
					.BindBuffer(5, &mbufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_TASK_BIT_NV)
					.Build(globalSet);
				VkDescriptorSet imageSet;
				DescriptorBuilder::Begin(&s_Cache->caches[currentFrame], &s_Cache->allocators[currentFrame])
					.BindImage(1, &imageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
					.BindBuffer(2, &lightBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
					.Build(imageSet);


				s_Cache->constant.meshletCount = static_cast<uint32_t>(mdlCmd.submesh->GetMeshlets().size());
				s_Cache->constant.padd = s_RenderDebugState;
				vkCmdPushConstants(commandBuffer, PipelineLibrary::Get("MeshShaders")->GetLayout(), VK_SHADER_STAGE_TASK_BIT_NV | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Constant), &s_Cache->constant);

				std::array<VkDescriptorSet, 2> sets = { globalSet, imageSet };
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLibrary::Get("MeshShaders")->GetLayout(), 0, static_cast<uint32_t>(sets.size()), sets.data(), 0, nullptr);
			

				PFN_vkCmdDrawMeshTasksIndirectNV func = (PFN_vkCmdDrawMeshTasksIndirectNV)vkGetDeviceProcAddr(device.GetHandle(), "vkCmdDrawMeshTasksIndirectNV");
				if (func != nullptr)
				{
					Profiler::PofileDataAdd("MeshletCount", mdlCmd.submesh->GetMeshlets().size());
					Profiler::PofileDataAdd("TriangleCount", mdlCmd.submesh->GetIndices().size());
					TN_PROFILE_GPU_EVENT("Draw");
					func(commandBuffer, s_Cache->indirectCmdBuffer->GetAllocation().buffer, i * sizeof(VkDrawMeshTasksIndirectCommandNV), 1, sizeof(VkDrawMeshTasksIndirectCommandNV));
				}
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
			TN_PROFILE_GPU_EVENT("GPU: Queue submit");
			TN_VK_CHECK(vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, swapchain.GetInFlight(currentFrame)));
		}
		{
			TN_PROFILE_SCOPE("swapchain present");
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
		s_Cache->meshCmds.Reset();
	}


	void Renderer::Shutdown()
	{
		for (size_t i = 0; i < g_FramesInFlight; ++i)
		{
			s_Cache->allocators[i].Shutdown();
			s_Cache->caches[i].Shutdown();
		}
		s_Cache->mainFB->CleanUp();
	}

}
