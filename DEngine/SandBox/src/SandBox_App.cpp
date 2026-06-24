#include "DEngine.h"

class SandBox : public DEngine::Application
{
public:
    SandBox() : Application() 
    {
        PushLayer(new DEngine::ImGuiLayer());
    }

    virtual ~SandBox() {}
};

DEngine::Application* DEngine::CreateApplication()
{
	return new SandBox();
}
