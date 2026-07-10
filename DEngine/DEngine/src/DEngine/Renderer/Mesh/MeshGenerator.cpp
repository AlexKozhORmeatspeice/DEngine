#include "dpch.h"
#include "MeshGenerator.h"

#include "DEngine/Core.h"

namespace DEngine
{
	Ref<Mesh> MeshGenerator::CreatePrimitive(PrimitiveType type)
	{
		switch (type)
		{
			case DEngine::PrimitiveType::Cube: return CreateCube();
		}
		
		D_CORE_ASSERT(false, "Can't create mesh with choosed primitive type");
		return nullptr;
	}

	Ref<Mesh> MeshGenerator::CreateCube()
	{
		BufferLayout layout = 
		{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float3, "a_Normal"},
			{ShaderDataType::Float2, "a_Texcoord"},
			{ShaderDataType::Float3, "a_Tangent"},
		};
		float verts[8 * 11] = {
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  // 0
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,  // 1
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,  // 2
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  // 3
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  // 4
			 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f,  // 5
			 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 0.0f,  // 6
			-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  // 7
		};

		uint32_t inds[36] = {
			0, 1, 2,
			2, 3, 0,
			4, 6, 5,
			4, 7, 6,
			3, 7, 0,
			7, 4, 0,
			1, 5, 6,
			1, 6, 2,
			3, 2, 6,
			3, 6, 7,
			0, 5, 1,
			0, 4, 5
		};

		Ref<Mesh> mesh = CreateRef<Mesh>(layout, verts, sizeof(verts), inds, sizeof(inds) / sizeof(uint32_t));

		return mesh;
	}
}