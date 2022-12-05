#pragma once

#include <glm/glm.hpp>
#include "Titan/Scene/Components.h"
#include <Titan/Rendering/Renderer.h>

void CalculateMatrixTransformSystem(TransformComponent& tf)
{
	const auto pos = glm::translate(glm::mat4(1.0f), tf.position);
	const auto rot = glm::mat4_cast(glm::tquat<float>(tf.rotation));
	const auto scale = glm::scale(glm::mat4(1.0f), tf.scale);
	tf.matrix = pos * rot * scale;
}

void LightSystem(LightComponent& lgh, TransformComponent& tf)
{
	Titan::LightCmd lightCmd{};
	lightCmd.color = glm::vec4(lgh.color, lgh.intensity);
	glm::vec3 dir = glm::tquat<float>(tf.rotation) * glm::vec3{ 0,0, -1 };
	lightCmd.direction =  glm::vec4(dir, 1.f);
	Titan::Renderer::Submit(lightCmd);
}