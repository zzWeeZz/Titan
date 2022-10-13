#pragma once
#include "Titan/Assets/Resource.h"
namespace Titan
{
	class ModelHandle : public Resource
	{
	public:
		ModelHandle() = default;
		virtual void Initialize(const std::filesystem::path& path) override;

		static Ref<ModelHandle> Create() { return CreateRef<ModelHandle>(); }
	private:

	};
}
