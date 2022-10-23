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
		static float time = 0;
		time += 0.004f;
		////tf.position.x = sinf(time);
		tf.quaternion = glm::quat(cos(glm::radians(time / 2) * 10), 0, sin(glm::radians(time / 2) * 10) * 1, 0);
		//tf.scale.x = (sinf(time) + 1.f) / 2.f;
		//tf.scale.y = (sinf(time) + 1.f) / 2.f;
		//tf.scale.z = (sinf(time) + 1.f) / 2.f;

		MeshCmd cmd{};
		cmd.package = ResourceRegistry::GetItem<ModelHandle>(mdl.modelHandle)->GetVertexPackage();
		auto pos = glm::translate(glm::mat4(1.0f), tf.position);
		auto rot = glm::toMat4(tf.quaternion);
		auto scale = glm::scale(glm::mat4(1.0f), tf.scale);
		cmd.transform = scale * pos * rot;
		Renderer::Submit(cmd);
	}
}