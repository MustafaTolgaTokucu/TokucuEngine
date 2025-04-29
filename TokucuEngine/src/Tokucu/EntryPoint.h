#pragma once

#ifdef TKC_PLATFORM_WINDOWS

extern Tokucu::Application* Tokucu::CreateApplication();

int main(int argc, char** argv)
{
	Tokucu::Log::Init();
	
	TKC_WARN("Welcome to TOKUCU Renderer");
	
	auto app = Tokucu::CreateApplication();
	app->Run();
	delete app;
	TKC_CORE_ERROR("Application is Closed");
}

#endif // TKC_PLATFORM_WINDOWS
