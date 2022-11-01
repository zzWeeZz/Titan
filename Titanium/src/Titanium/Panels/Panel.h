#pragma once
namespace Titan
{
	class Panel
	{
	public:
		Panel() {};
		virtual ~Panel(){}
		virtual void OnImGuiDraw() = 0;

	};
}