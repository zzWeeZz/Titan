#pragma once

#include "Titan/Scene/Components.h"
#include <Titan/Rendering/Renderer.h>

void LightSystem(LightComponent& lgh, TransformComponent& tf)
{
	Titan::LightCmd lightCmd{};
	lightCmd.color = glm::vec4(lgh.color, lgh.intensity);
	glm::vec3 dir = tf.quaternion * glm::vec3{ 0,0, -1 };
	lightCmd.direction =  glm::vec4(dir, 1.f);
	Titan::Renderer::Submit(lightCmd);
}