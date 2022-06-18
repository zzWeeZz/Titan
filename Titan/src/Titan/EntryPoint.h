#pragma once


extern Titan::Application* Titan::CreateApplication();

int main(int argc, char** argv)
{
	Titan::Log::Initialize();
	TITAN_CORE_TRACE("TITAN ENGINE");
	auto app = Titan::CreateApplication();
	app->Run();
	delete app;
	return 0;
}