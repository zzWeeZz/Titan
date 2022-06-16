#pragma once

extern Titan::Application* Titan::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Titan::CreateApplication();
	app->Run();
	delete app;
	return 0;
}