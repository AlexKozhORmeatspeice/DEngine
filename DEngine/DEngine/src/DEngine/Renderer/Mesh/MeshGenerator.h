#pragma once

#include "DEngine/Core.h"
#include "Mesh.h"

namespace DEngine
{
	enum class PrimitiveType { Cube };

	class MeshGenerator
	{
	public:
		static Ref<Mesh> CreatePrimitive(PrimitiveType type);
		
	private:
		static Ref<Mesh> CreateCube();
	};
}
