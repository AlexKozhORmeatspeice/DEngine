#include "DEngine.h"
#include "DEngine/EntryPoint.h"

#include "EditorLayer.h"

namespace DEngine
{
	class Editor : public Application
	{
	public:
		Editor() : Application("DEngine editor")
		{
			PushLayer(new EditorLayer());
		}

		virtual ~Editor() {}
	};

	Application* CreateApplication()
	{
		return new Editor();
	}
}
