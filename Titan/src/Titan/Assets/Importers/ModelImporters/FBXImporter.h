#pragma once
#include "OpenFBX/src/ofbx.h"

namespace Titan
{
	class Submesh;
	class FBXImporter
	{
	public:
		static void Import(const std::filesystem::path& filepath, std::vector<Submesh>& outVertex);
	private:
		static void LoadScene(ofbx::IScene* scene, std::vector<Submesh>& outSubmeshes);
	};
}