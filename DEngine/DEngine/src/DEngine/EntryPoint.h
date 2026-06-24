#pragma once

#ifdef D_PLATFORM_WINDOWS

extern DEngine::Application* DEngine::CreateApplication();

int main(int argc, char** argv)
{
	DEngine::DLog::Init();
	
	DEngine::Application* app = DEngine::CreateApplication();
	app->Run();
	delete app;
}

#endif
