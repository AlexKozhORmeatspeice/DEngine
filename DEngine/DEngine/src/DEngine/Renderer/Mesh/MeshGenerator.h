#pragma once

#include "DEngine/Core.h"
#include "Mesh.h"

#include "filesystem"

namespace DEngine
{
	enum class PrimitiveType { Cube };

	static std::string PrimitiveTypeToString(PrimitiveType type)
	{
		switch (type)
		{
			case PrimitiveType::Cube:
				return "CubePrimitive";
		}

		return "UnkownPrimitive";
	}

	class MeshGenerator
	{
	public:
		static Ref<Mesh> CreatePrimitive(PrimitiveType type);
		static std::filesystem::path ConstructPrimitivePath(PrimitiveType type);
	private:
		static Ref<Mesh> CreateCube();
	};
}
