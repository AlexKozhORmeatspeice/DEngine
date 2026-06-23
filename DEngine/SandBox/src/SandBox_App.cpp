#include "DEngine.h"

class SandBox : public DEngine::Application
{
public:
    SandBox() {}
    virtual ~SandBox() {}
};

DEngine::Application* DEngine::CreateApplication()
{
	return new SandBox();
}
