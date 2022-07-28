#pragma once
#include "Titan/Core/Core.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "Serializer.hpp"
#include "Snowflake.hpp"
#include "Titan/Scene/Components.h"

namespace Titan
{
	COMPONENT(CameraComponent)
	{
		REGISTER_COMPONENT("{279CBD02-B929-4073-B161-DB0D9E08F84C}"_guid);
		bool IsPerspective = true;
		bool IsDirty = false;

		float FOV = 90.f;
		float AspectRatio = 1700.f/900.f;
		float size = 1.f;

		// camera maths
		glm::mat4 Projection;
		glm::mat4 View;
	};


	void CameraSystem(CameraComponent& c, TransformComponent& tf);

	/*class Camera
	{
	public:
		Camera(PerspectiveSpecification& );
		Camera(OrthographicSpecification& );
		static Ref<Camera> Create(PerspectiveSpecification& );
		static Ref<Camera> Create(OrthographicSpecification& );
	private:
	};*/
}

