#pragma once
#include "Titan/Layer.h"
namespace Titan
{
	class ImGuiLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnUpdate() override;
		void OnDetach() override;
		void OnEvent(Event& e) override;
	private:
	};
}