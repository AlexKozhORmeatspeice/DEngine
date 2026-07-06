#include "dpch.h"
#include "MeshGenerator.h"

namespace DEngine
{
	//TODO: так-то это неплохо бы как-то кэшировать. Но думаю это стоит делать после появления системы ассетов уже
	Ref<Mesh> MeshGenerator::CreateCube()
	{
		float verts[8 * 9] = {
			-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // 0
			 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f,  // 1
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f,  // 2
			-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f,  // 3
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f,  // 4
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  // 5
			 0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f, 1.0f,  1.0f, 1.0f,  // 6
			-0.5f,  0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  0.0f, 1.0f   // 7
		};

		BufferLayout layout =
		{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexCoord"},
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