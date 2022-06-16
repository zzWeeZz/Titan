#include "Titan.h"

class SandBox : public Titan::Application
{
public:
	SandBox()
	{

	}

	~SandBox()
	{

	}
};

Titan::Application* Titan::CreateApplication()
{
	return new SandBox();
}