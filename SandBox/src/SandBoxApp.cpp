#include "Titan.h"
#include "GLFW/glfw3.h"

class SandBox : public Titan::Application
{
public:
	

	~SandBox()
	{

	}
private:
	GLFWwindow* window;
};

Titan::Application* Titan::CreateApplication()
{
	return new SandBox();
}