#include "DEngine.h"

#include "imgui/imgui.h"

class SandBox : public DEngine::Application
{
public:
    SandBox() : Application() 
    {
        //WIP
    }

    virtual ~SandBox() {}
};

DEngine::Application* DEngine::CreateApplication()
{
	return new SandBox();
}
