#include "dpch.h"
#include "RenderCommand.h"

//TODO: TEMP
#include "DEngine/Platform/OpenGL/OpenGLRendererAPI.h"

namespace DEngine
{
	//TODO: перенести создание в отдельный обработчик
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}