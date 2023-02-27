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
#include "Titan/Rendering/Libraries/ShaderLibrary.h"

#define MAX_INDIRECT_COMMANDS 20000
#define MAX_MESHLETS 100000
#define MAX_VERTICES 20000000
#define MAX_TRIANGLES 100000000
#define MAX_TEXTURES 1000
#define MAX_MESHES 100000

namespace Titan
{
	struct Constant
	{
		uint32_t meshletCount;
		uint32_t vertexCount;
		uint32_t indexCount;
		uint32_t renderDebugState;
	};

	struct Mesh
	{
		uint32_t meshletOffset;
		uint32_t vertexOffset;
		uint32_t triangleOffset;
		uint32_t vertexIndexOffset;
		glm::mat4 transform;
	};

	struct Cache
	{
		// Structs
		CameraCmd currentCamera{};
		LightCmd lightData{};
		MemoryVector<MeshCmd> meshCmds;
		CameraData cameraData{};
		ModelData modelData{};
		Constant constant{};

		// Vk objects
		Ref<Framebuffer> mainFB;
		Ref<UniformBuffer> cameraBuffer;
		Ref<UniformBuffer> lightBuffer;
		Ref<GenericBuffer> indirectCmdBuffer;
		Ref<GenericBuffer> meshletBuffer;
		Ref<GenericBuffer> vertexBuffer;
		Ref<GenericBuffer> triangleBuffer;
		Ref<GenericBuffer> vertexIndexBuffer;
		Ref<GenericBuffer> meshBuffer;

		// In-flight data
		PerFrameInFlight<DescriptorAllocator> allocators;
		PerFrameInFlight<DescriptorAllocator> bindlessAllocators;
		PerFrameInFlight<DescriptorLayoutCache> caches;
		PerFrameInFlight<DescriptorLayoutCache> bindlessCaches;
		PerFrameInFlight<VkDescriptorSet> bindlessSets{};
		std::unordered_map<uint32_t, MeshCmd> dirtyMeshCommands;
		PerFrameInFlight<bool> isFrameDirty{};
		PerFrameInFlight<bool> isFrameDirtyValidator{};
		PerFrameInFlight<size_t> meshCountPreviousFrame{};

		// Bind registry
		BindRegistry textureBindSet;
		BindRegistry meshletBindSet;
		BindRegistry vertexBindSet;
		BindRegistry triangleBindSet;
		BindRegistry vertexIndexBindSet;
	};
	static Scope<Cache> s_Cache = CreateScope<Cache>();
	static uint32_t s_RenderDebugState;

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
		if (meshCmd.dirty)
		{
			s_Cache->dirtyMeshCommands[static_cast<uint32_t>(s_Cache->meshCmds.Size() - 1)] = meshCmd;
			for (size_t i = 0; i < g_FramesInFlight; i++)
			{
				s_Cache->isFrameDirty[i] = true;
			}
		}
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

		GraphicsPipelineInfo info{};

		info.topology = Topology::TriangleList;
		info.imageFormats = { ImageFormat::R8G8B8A8_UN, ImageFormat::D32_SF_S8_UI };

		info.tsPath = "Engine/Shaders/Mesh_ts.hlsl";
		info.msPath = "Engine/Shaders/Mesh_ms.hlsl";
		info.fsPath = "Engine/Shaders/Mesh_ps.hlsl";
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

		CreateMeshletBuffer();
		CreateVertexBuffer();
		CreateTriangleBuffer();
		CreateVertexIndexBuffer();
		CreateMeshBuffer();

		SamplerLibrary::Add("Clamp", Filter::Linear, Address::ClampToEdge, MipmapMode::Linear);
	}

	void Renderer::CreateMeshletBuffer()
	{
		GenericBufferInfo meshletBufferInfo{};
		meshletBufferInfo.data = nullptr;
		meshletBufferInfo.size = MAX_MESHLETS;
		meshletBufferInfo.stride = sizeof(Meshlet);
		meshletBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		meshletBufferInfo.allocUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		meshletBufferInfo.perFrameInFlight = false;

		s_Cache->meshletBuffer = GenericBuffer::Create(meshletBufferInfo);
	}
	void Renderer::CreateVertexBuffer()
	{
		GenericBufferInfo vertexBufferInfo{};
		vertexBufferInfo.data = nullptr;
		vertexBufferInfo.size = MAX_VERTICES;
		vertexBufferInfo.stride = sizeof(BufferVertex);
		vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		vertexBufferInfo.allocUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		vertexBufferInfo.perFrameInFlight = false;

		s_Cache->vertexBuffer = GenericBuffer::Create(vertexBufferInfo);
	}
	void Renderer::CreateTriangleBuffer()
	{
		GenericBufferInfo triangleBufferInfo{};
		triangleBufferInfo.data = nullptr;
		triangleBufferInfo.size = MAX_TRIANGLES;
		triangleBufferInfo.stride = sizeof(uint32_t);
		triangleBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		triangleBufferInfo.allocUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		triangleBufferInfo.perFrameInFlight = false;

		s_Cache->triangleBuffer = GenericBuffer::Create(triangleBufferInfo);
	}
	void Renderer::CreateVertexIndexBuffer()
	{
		GenericBufferInfo triangleBufferInfo{};
		triangleBufferInfo.data = nullptr;
		triangleBufferInfo.size = MAX_TRIANGLES;
		triangleBufferInfo.stride = sizeof(uint32_t);
		triangleBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		triangleBufferInfo.allocUsage = VMA_MEMORY_USAGE_GPU_ONLY;
		triangleBufferInfo.perFrameInFlight = false;

		s_Cache->vertexIndexBuffer = GenericBuffer::Create(triangleBufferInfo);
	}
	void Renderer::CreateMeshBuffer()
	{
		GenericBufferInfo bufferInfo{};
		bufferInfo.data = nullptr;
		bufferInfo.size = MAX_MESHES;
		bufferInfo.stride = sizeof(Mesh);
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		bufferInfo.allocUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		bufferInfo.perFrameInFlight = true;

		s_Cache->meshBuffer = GenericBuffer::Create(bufferInfo);
	}

	void Renderer::WaitOnNewFrame()
	{
		TN_PROFILE_FUNCTION();
		auto& currentFrame = GraphicsContext::GetCurrentFrame();
		auto& swapchain = GraphicsContext::GetSwapchain();
		auto& device = GraphicsContext::GetDevice();
		vkWaitForFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame), VK_TRUE, UINT64_MAX);
		s_Cache->allocators[currentFrame].ResetPools();
		TitanImGui::FlushDescriptors(currentFrame);

		bool shouldCleanUp = true;
		for (size_t i = 0; i < g_FramesInFlight; i++)
		{
			if (s_Cache->isFrameDirtyValidator[i])
			{
				shouldCleanUp = false;
				break;
			}
			s_Cache->isFrameDirtyValidator[i] = false;
		}

		if (shouldCleanUp)
		{
			for (size_t i = 0; i < s_Cache->meshCmds.Size(); ++i)
			{
				auto& cmd = s_Cache->meshCmds[i];
				cmd.submesh->GetTranform() = cmd.transform;
			}
		}

		s_Cache->dirtyMeshCommands.clear();
		s_Cache->meshCountPreviousFrame[currentFrame] = s_Cache->meshCmds.Size();
		s_Cache->meshCmds.Reset();
	}

	void Renderer::Begin()
	{
		TN_PROFILE_FUNCTION();
		Profiler::PofileDataSet("MeshletCount", size_t(0));
		Profiler::PofileDataSet("TriangleCount", size_t(0));

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
		uint32_t imageIndex;
		{
			TN_PROFILE_SCOPE("Reset Current Fence");
			vkResetFences(device.GetHandle(), 1, &swapchain.GetInFlight(currentFrame));
			 imageIndex = index;
			vkResetCommandBuffer(commandBuffer, 0);
		}



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
		size_t meshletCount = 0;
		{
			TN_PROFILE_SCOPE("Count Meshlets");
			for (size_t i = 0; i < s_Cache->meshCmds.Size(); ++i)
			{
				meshletCount += s_Cache->meshCmds[i].submesh->GetMeshlets().size();
			}
		}
		if (!s_Cache->bindlessSets[currentFrame] || ValidateBindlessBuffers())
		{
			TN_PROFILE_SCOPE("Rebuild Bindless structure");
			CombineMeshlets(commandBuffer);
			CombineVertexBuffers(commandBuffer);
			CombineTriangleBuffers(commandBuffer);
			CombineVertexIndexBuffers(commandBuffer);
			s_Cache->bindlessAllocators[currentFrame].ResetPools();
			{
				VkDescriptorBufferInfo bindlessInfo{};
				bindlessInfo.buffer = s_Cache->meshletBuffer->GetAllocation().buffer;
				bindlessInfo.offset = 0;
				bindlessInfo.range = s_Cache->meshletBuffer->GetAllocation().sizeOfBuffer;

				VkDescriptorBufferInfo bindlessVertexInfo{};
				bindlessVertexInfo.buffer = s_Cache->vertexBuffer->GetAllocation().buffer;
				bindlessVertexInfo.offset = 0;
				bindlessVertexInfo.range = s_Cache->vertexBuffer->GetAllocation().sizeOfBuffer;

				VkDescriptorBufferInfo bindlessTriangleInfo{};
				bindlessTriangleInfo.buffer = s_Cache->triangleBuffer->GetAllocation().buffer;
				bindlessTriangleInfo.offset = 0;
				bindlessTriangleInfo.range = s_Cache->triangleBuffer->GetAllocation().sizeOfBuffer;

				VkDescriptorBufferInfo bindlessVertexIndexInfo{};
				bindlessVertexIndexInfo.buffer = s_Cache->vertexIndexBuffer->GetAllocation().buffer;
				bindlessVertexIndexInfo.offset = 0;
				bindlessVertexIndexInfo.range = s_Cache->vertexIndexBuffer->GetAllocation().sizeOfBuffer;

				DescriptorBuilder::Begin(&s_Cache->bindlessCaches[currentFrame], &s_Cache->bindlessAllocators[currentFrame])
					//.BindBuffer(0, &bindlessInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_TASK_BIT_NV)
					.BindBuffer(1, &bindlessInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_MESH_BIT_NV)
					.BindBuffer(2, &bindlessVertexInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_MESH_BIT_NV)
					.BindBuffer(3, &bindlessTriangleInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_MESH_BIT_NV)
					.BindBuffer(4, &bindlessVertexIndexInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_MESH_BIT_NV)
					.Build(s_Cache->bindlessSets[currentFrame]);
			}


			void* mappedMemory = nullptr;
			TitanAllocator::MapMemory(s_Cache->indirectCmdBuffer->GetAllocation(), mappedMemory);
			auto drawCommands = (VkDrawMeshTasksIndirectCommandNV*)mappedMemory;

			drawCommands[0].taskCount = static_cast<uint32_t>(meshletCount);
			drawCommands[0].firstTask = 0;
			TitanAllocator::UnMapMemory(s_Cache->indirectCmdBuffer->GetAllocation());
		}
		bool shouldUpdateMeshBuffer = s_Cache->isFrameDirty[currentFrame] || s_Cache->meshCountPreviousFrame[currentFrame] != s_Cache->meshCmds.Size();
		if (shouldUpdateMeshBuffer)
		{
			TN_PROFILE_SCOPE("Update Mesh buffer");
			UpdateMeshBuffer();
			s_Cache->isFrameDirty[currentFrame] = false;
			s_Cache->isFrameDirtyValidator[currentFrame] = false;
		}
		else
		{
			s_Cache->isFrameDirtyValidator[currentFrame] = true;
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
		const VkRect2D rec = s_Cache->mainFB->GetRect();
		
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
			if (s_Cache->meshCmds.HasData())
			{
				auto& mdlCmd = s_Cache->meshCmds[0];
				auto texture = ResourceRegistry::GetItem<Texture>(mdlCmd.textureId);
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = texture->GetView();
				imageInfo.sampler = texture->GetSampler();

				s_Cache->cameraData.mdlSpace = mdlCmd.transform;
				s_Cache->cameraBuffer->SetData(&s_Cache->cameraData, sizeof(CameraData));

				auto& light = s_Cache->lightBuffer;

				VkDescriptorBufferInfo lightBufferInfo{};
				lightBufferInfo.buffer = light->GetAllocation().buffer;
				lightBufferInfo.offset = 0;
				lightBufferInfo.range = light->GetAllocation().sizeOfBuffer;

				VkDescriptorBufferInfo modelBufferInfo{};
				modelBufferInfo.buffer = s_Cache->meshBuffer->GetAllocation().buffer;
				modelBufferInfo.offset = 0;
				modelBufferInfo.range = s_Cache->meshBuffer->GetAllocation().sizeOfBuffer;

				VkDescriptorSet globalSet;
				DescriptorBuilder::Begin(&s_Cache->caches[currentFrame], &s_Cache->allocators[currentFrame])
					.BindBuffer(0, &bufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_MESH_BIT_NV)
					.BindBuffer(1, &modelBufferInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_MESH_BIT_NV)
					.BindImage(5, &imageInfo, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
					.BindBuffer(6, &lightBufferInfo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
					.Build(globalSet);


				s_Cache->constant.meshletCount = static_cast<uint32_t>(meshletCount);
				s_Cache->constant.renderDebugState = s_RenderDebugState;

				vkCmdPushConstants(commandBuffer, PipelineLibrary::Get("MeshShaders").lock()->GetLayout(), VK_SHADER_STAGE_TASK_BIT_NV /*| VK_SHADER_STAGE_MESH_BIT_NV */| VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Constant), &s_Cache->constant);
				std::array<VkDescriptorSet, 2> sets = { globalSet, s_Cache->bindlessSets[currentFrame] };
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLibrary::Get("MeshShaders").lock()->GetLayout(), 0, static_cast<uint32_t>(sets.size()), sets.data(), 0, nullptr);


				PFN_vkCmdDrawMeshTasksIndirectNV func = (PFN_vkCmdDrawMeshTasksIndirectNV)vkGetDeviceProcAddr(device.GetHandle(), "vkCmdDrawMeshTasksIndirectNV");
				if (func != nullptr)
				{
					Profiler::PofileDataAdd("MeshletCount", meshletCount);
					Profiler::PofileDataAdd("TriangleCount", mdlCmd.submesh->GetIndices().size());
					func(commandBuffer, s_Cache->indirectCmdBuffer->GetAllocation().buffer, 0 * sizeof(VkDrawMeshTasksIndirectCommandNV), 1, sizeof(VkDrawMeshTasksIndirectCommandNV));
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
	}

	bool Renderer::ValidateBindlessBuffers()
	{
		TN_PROFILE_FUNCTION();
		for (size_t i = 0; i < s_Cache->meshCmds.Size(); ++i)
		{
			auto& mdlCmd = s_Cache->meshCmds[i];
			uint32_t index = 0;
			if (!s_Cache->vertexBindSet.Exists(mdlCmd.submesh->GetID(), index))
			{
				return true;
			}
			if (!s_Cache->meshletBindSet.Exists(mdlCmd.submesh->GetID(), index))
			{
				return true;
			}
		}
		return false;
	}

	bool Renderer::ValidateBindlessTextures()
	{
		for (size_t i = 0; i < s_Cache->meshCmds.Size(); ++i)
		{
			auto& mdlCmd = s_Cache->meshCmds[i];
			uint32_t index = 0;
			if (!s_Cache->triangleBindSet.Exists(mdlCmd.submesh->GetID(), index))
			{
				return true;
			}
		}
		return false;
	}

	void Renderer::CombineMeshlets(VkCommandBuffer& cmd)
	{
		auto dstMeshletBuffer = s_Cache->meshletBuffer->GetAllocation().buffer;
		VkBufferCopy copy{};
		VkDeviceSize dstOffset = 0;
		VkBuffer srcBuffer{};
		for (uint32_t i = 0; i < s_Cache->meshCmds.Size(); ++i)
		{
			auto& mdlCmd = s_Cache->meshCmds[i];
			auto sizeAndStride = mdlCmd.submesh->GetMeshletBuffer()->GetAllocation().sizeOfBuffer;
			srcBuffer = mdlCmd.submesh->GetMeshletBuffer()->GetAllocation().buffer;

			void* mappedMem = nullptr;
			TitanAllocator::MapMemory(mdlCmd.submesh->GetMeshletBuffer()->GetAllocation(), mappedMem);

			Meshlet* meshletMappedMem = (Meshlet*)(mappedMem);
			for (size_t j = 0; j < mdlCmd.submesh->GetMeshlets().size(); j++)
			{
				meshletMappedMem[j].meshId = i;
			}
			TitanAllocator::UnMapMemory(mdlCmd.submesh->GetMeshletBuffer()->GetAllocation());


			copy.dstOffset = dstOffset;
			copy.srcOffset = 0;
			copy.size = sizeAndStride;
			vkCmdCopyBuffer(cmd, srcBuffer, dstMeshletBuffer, 1, &copy);

			const bool isZero = dstOffset == 0;
			const uint32_t sizeOfMeshlet = static_cast <uint32_t>(sizeof(Meshlet));
			auto offset = !isZero ? static_cast<uint32_t>(dstOffset) / sizeOfMeshlet : static_cast<uint32_t>(dstOffset);
			s_Cache->meshletBindSet.Register(mdlCmd.submesh->GetID(), offset);
			dstOffset += sizeAndStride;
		}
	}

	void Renderer::CombineVertexBuffers(VkCommandBuffer& cmd)
	{
		auto dstVertexBuffer = s_Cache->vertexBuffer->GetAllocation().buffer;
		VkBufferCopy copy{};
		VkDeviceSize dstOffset = 0;
		VkBuffer srcBuffer{};
		for (size_t i = 0; i < s_Cache->meshCmds.Size(); ++i)
		{
			auto& mdlCmd = s_Cache->meshCmds[i];
			auto sizeAndStride = mdlCmd.submesh->GetVertexBuffer()->GetAllocation().sizeOfBuffer;
			srcBuffer = mdlCmd.submesh->GetVertexBuffer()->GetAllocation().buffer;

			copy.dstOffset = dstOffset;
			copy.srcOffset = 0;
			copy.size = sizeAndStride;
			vkCmdCopyBuffer(cmd, srcBuffer, dstVertexBuffer, 1, &copy);
			const bool isZero = dstOffset == 0;
			const uint32_t sizeOfVertex = static_cast <uint32_t>(sizeof(BufferVertex));
			auto offset = !isZero ? static_cast<uint32_t>(dstOffset) / sizeOfVertex : static_cast<uint32_t>(dstOffset);
			s_Cache->vertexBindSet.Register(mdlCmd.submesh->GetID(), offset);
			dstOffset += sizeAndStride;
		}
	}

	void Renderer::CombineTriangleBuffers(VkCommandBuffer& cmd)
	{
		auto dstVertexBuffer = s_Cache->triangleBuffer->GetAllocation().buffer;
		VkBufferCopy copy{};
		VkDeviceSize dstOffset = 0;
		VkBuffer srcBuffer{};
		for (size_t i = 0; i < s_Cache->meshCmds.Size(); ++i)
		{
			auto& mdlCmd = s_Cache->meshCmds[i];
			auto sizeAndStride = mdlCmd.submesh->GetTriangleBuffer()->GetAllocation().sizeOfBuffer;
			srcBuffer = mdlCmd.submesh->GetTriangleBuffer()->GetAllocation().buffer;

			copy.dstOffset = dstOffset;
			copy.srcOffset = 0;
			copy.size = sizeAndStride;
			vkCmdCopyBuffer(cmd, srcBuffer, dstVertexBuffer, 1, &copy);
			const bool isZero = dstOffset == 0;
			const uint32_t sizeOfTriangle = static_cast <uint32_t>(sizeof(uint32_t));
			auto offset = !isZero ? static_cast<uint32_t>(dstOffset) / sizeOfTriangle : static_cast<uint32_t>(dstOffset);
			s_Cache->triangleBindSet.Register(mdlCmd.submesh->GetID(), offset);
			dstOffset += sizeAndStride;
		}
	}

	void Renderer::CombineVertexIndexBuffers(VkCommandBuffer& cmd)
	{
		auto dstVertexBuffer = s_Cache->vertexIndexBuffer->GetAllocation().buffer;
		VkBufferCopy copy{};
		VkDeviceSize dstOffset = 0;
		VkBuffer srcBuffer{};
		for (size_t i = 0; i < s_Cache->meshCmds.Size(); ++i)
		{
			auto& mdlCmd = s_Cache->meshCmds[i];
			auto sizeAndStride = mdlCmd.submesh->GetMeshletVertexBuffer()->GetAllocation().sizeOfBuffer;
			srcBuffer = mdlCmd.submesh->GetMeshletVertexBuffer()->GetAllocation().buffer;

			copy.dstOffset = dstOffset;
			copy.srcOffset = 0;
			copy.size = sizeAndStride;
			vkCmdCopyBuffer(cmd, srcBuffer, dstVertexBuffer, 1, &copy);
			const bool isZero = dstOffset == 0;
			const uint32_t sizeOfTriangle = static_cast <uint32_t>(sizeof(uint32_t));
			auto offset = !isZero ? static_cast<uint32_t>(dstOffset) / sizeOfTriangle : static_cast<uint32_t>(dstOffset);
			s_Cache->vertexIndexBindSet.Register(mdlCmd.submesh->GetID(), offset);
			dstOffset += sizeAndStride;
		}
	}

	void Renderer::UpdateMeshBuffer()
	{
		TN_PROFILE_FUNCTION();

		void* mappedMem = nullptr;
		TitanAllocator::MapMemory(s_Cache->meshBuffer->GetAllocation(), mappedMem);

		auto* mappedMeshArray = reinterpret_cast<Mesh*>(mappedMem);

		for (size_t i = 0; i < s_Cache->meshCmds.Size(); ++i)
		{
			auto& mesh = mappedMeshArray[i];
			auto& mdlCmd = s_Cache->meshCmds[i];
			mesh.transform = mdlCmd.transform;
			mesh.meshletOffset = s_Cache->meshletBindSet.Fetch(mdlCmd.submesh->GetID());
			mesh.vertexOffset = s_Cache->vertexBindSet.Fetch(mdlCmd.submesh->GetID());
			mesh.triangleOffset = s_Cache->triangleBindSet.Fetch(mdlCmd.submesh->GetID());
			mesh.vertexIndexOffset = s_Cache->vertexIndexBindSet.Fetch(mdlCmd.submesh->GetID());
		}

		TitanAllocator::UnMapMemory(s_Cache->meshBuffer->GetAllocation());
	}

	void Renderer::Shutdown()
	{
		for (size_t i = 0; i < g_FramesInFlight; ++i)
		{
			s_Cache->bindlessAllocators[i].ResetPools();
			s_Cache->bindlessAllocators[i].Shutdown();
			s_Cache->bindlessCaches[i].Shutdown();
			s_Cache->allocators[i].Shutdown();
			s_Cache->caches[i].Shutdown();
		}
		s_Cache->mainFB->CleanUp();
	}

}
