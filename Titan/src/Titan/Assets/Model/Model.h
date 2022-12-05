#pragma once
#include "Snowflake.hpp"
#include "Titan/Core/Core.h"
#include "Titan/Scene/Components.h"
#include "Titan/Assets/ResourceRegistry.h"
#include <Titan/Assets/Model/ModelHandle.h>
#include <Titan/Rendering/Renderer.h>
#include <glm/gtx/quaternion.hpp>
#include "Titan/Assets/Model/Submesh.h"
namespace Titan
{
	COMPONENT(ModelComponent)
	{
		REGISTER_COMPONENT("{564BFFF1-275D-48B8-B94B-32E3A9B138A9}"_guid);
		TitanID modelHandle = {};
		TitanID textureHandle = {};
	};

	inline void ModelSystem(ModelComponent& mdl, TransformComponent& tf)
	{
		auto resource = ResourceRegistry::GetItem<ModelHandle>(mdl.modelHandle);
		for (auto& submesh : resource->GetSubMeshes())
		{
			MeshCmd cmd{};
			cmd.package = submesh.GetVertexPackage();
			cmd.textureId = mdl.textureHandle;
			cmd.transform = tf.matrix;
			Renderer::Submit(cmd);
		}
	}
}