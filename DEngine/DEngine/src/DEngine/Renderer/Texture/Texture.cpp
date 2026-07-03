#include "dpch.h"
#include "Texture.h"
#include "DEngine/Application.h"
#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Platform/OpenGL/OpenGLTexture.h"

namespace DEngine
{
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(path);
			case RendererAPI::API::None: D_CORE_ASSERT(false, "None API is not supported");
			default:
				break;
		}

		D_CORE_ASSERT(false, "Current Renderer API doesn't support textures");
		return nullptr;
	}
}