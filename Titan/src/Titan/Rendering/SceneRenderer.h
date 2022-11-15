#pragma once
namespace Snowflake
{
	class Registry;
}

namespace Titan
{
	class SceneRenderer
	{
	public:
		SceneRenderer(Snowflake::Registry& reg);
		void Render();
	private:
		Snowflake::Registry& m_Registry;
	};
}