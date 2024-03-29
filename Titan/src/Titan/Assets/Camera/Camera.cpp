#include "TNpch.h"
#include "Camera.h"
#include <glm/gtx/quaternion.hpp>
#include "Titan/Rendering/Renderer.h"
namespace Titan
{
	void CameraSystem(CameraComponent& c, TransformComponent& tf)
	{
		if (c.IsPerspective)
		{
			c.Projection = glm::perspective(glm::radians(c.FOV), c.AspectRatio, 0.1f, 200.f);
			c.View = glm::translate(glm::mat4(1.f), tf.position);
			c.View *= glm::mat4_cast(glm::quat(tf.rotation));
		}
		else
		{
		}
		CameraCmd cmd{};
		cmd.view = c.View;
		cmd.proj = c.Projection;
		Renderer::Submit(cmd);
	}
}
