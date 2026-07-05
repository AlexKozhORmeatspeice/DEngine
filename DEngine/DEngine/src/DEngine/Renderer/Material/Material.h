#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Shader/Shader.h"

//Базовый класс всех материалов
namespace DEngine
{
	class Material
	{
	protected:
		Material() = default;
		Ref<Shader> m_Shader;
	};
}
