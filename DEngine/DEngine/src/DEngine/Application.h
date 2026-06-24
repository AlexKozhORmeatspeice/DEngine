#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace DEngine
{
    class D_API Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();
    };

    //To be defined in client
	Application* CreateApplication();
}