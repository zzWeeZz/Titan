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
		void Float(const std::string& label, float& value);
		void Color3(const std::string& label, glm::vec3& col);
		void Color4(const std::string& label, glm::vec4& col);
		void Quat(const std::string& label, glm::quat& quat);
		void String(const std::string& label, std::string& str);
		void String(const std::string& label, Snowflake::String& str);
	}
}