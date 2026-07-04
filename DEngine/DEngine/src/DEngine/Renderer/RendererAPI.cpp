#include "dpch.h"
#include "RendererAPI.h"

namespace DEngine
{
	//TODO: это надо как-то обрабатывать, потому что мы должны поддерживать разные API, но пока что так
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
}