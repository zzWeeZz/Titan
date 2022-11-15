#pragma once
#include "Snowflake.hpp"
#include "Titan/Core/Core.h"
#include "Titan/Scene/Components.h"
#include "Titan/Assets/ResourceRegistry.h"
#include <Titan/Assets/Model/ModelHandle.h>
#include <Titan/Rendering/Renderer.h>
#include <glm/gtx/quaternion.hpp>
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
		MeshCmd cmd{};
		cmd.package = ResourceRegistry::GetItem<ModelHandle>(mdl.modelHandle)->GetVertexPackage();
		cmd.textureId = mdl.textureHandle;
		auto pos = glm::translate(glm::mat4(1.0f), tf.position);
		auto rot = glm::toMat4(tf.quaternion);
		auto scale = glm::scale(glm::mat4(1.0f), tf.scale);
		cmd.transform = pos * rot * scale;
		Renderer::Submit(cmd);
	}
}