#pragma once
#include <string>

#include "glm/glm.hpp"
#include <glm/glm/gtc/quaternion.hpp>

namespace Snowflake
{
	class String;
}

namespace Titan
{
	namespace ImGuiUtils
	{
		void Vec3(const std::string& label, glm::vec3& vec);
		void Quat(const std::string& label, glm::quat& quat);
		void String(const std::string& label, std::string& str);
		void String(const std::string& label, Snowflake::String& str);
	}
}