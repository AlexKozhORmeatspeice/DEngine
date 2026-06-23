#include "Application.h"
#include "iostream"

namespace DEngine
{
    Application::Application()
    {

    }

    Application::~Application()
    {

    }

    void Application::Run()
    {
        while (true)
        {
			std::cout << "Running..." << std::endl;
        }
    }
}