#include "dpch.h"
#include "MeshGenerator.h"

#include "DEngine/Core.h"
#include "DEngine/Asset/Serializer/MeshSerializer.h"
#include "DEngine/Project/Project.h"

namespace DEngine
{
	const std::string PRIMITIVES_DIR_NAME = "primitives";

	Ref<Mesh> MeshGenerator::CreatePrimitive(PrimitiveType type)
	{
		switch (type)
		{
			case DEngine::PrimitiveType::Cube: return CreateCube();
		}
		
		D_CORE_ASSERT(false, "Can't create mesh with choosed primitive type");
		return nullptr;
	}

	std::filesystem::path MeshGenerator::ConstructPrimitivePath(PrimitiveType type)
	{
		std::filesystem::path meshPrimitivePath = Project::GetResourcesRegistryPath();
		meshPrimitivePath = meshPrimitivePath / PRIMITIVES_DIR_NAME;

		if (!std::filesystem::exists(meshPrimitivePath))
		{
			std::filesystem::create_directories(meshPrimitivePath);
			D_CORE_INFO("Created directory: {0}", meshPrimitivePath.string());
		}

		std::string primitiveName = PrimitiveTypeToString(type) + DMESH_FILE_EXT;
		meshPrimitivePath = meshPrimitivePath / primitiveName;

		return meshPrimitivePath;
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

		// 24 вершины (по 4 на каждую грань)
		float verts[24 * 11] = {
			// Передняя грань (Z+)
			-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f, // 0
			 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f, // 1
			 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f, // 2
			-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f, // 3

			// Задняя грань (Z-)
			-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,  -1.0f,  0.0f,  0.0f, // 4
			 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f, // 5
			 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,  -1.0f,  0.0f,  0.0f, // 6
			-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f, // 7

			// Левая грань (X-)
			-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f, // 8
			-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f,  0.0f, -1.0f, // 9
			-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f, // 10
			-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f,  0.0f, -1.0f, // 11

			// Правая грань (X+)
			 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f, // 12
			 0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f, // 13
			 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f, // 14
			 0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f, // 15

			 // Верхняя грань (Y+)
			 -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f, // 16
			  0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f, // 17
			  0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f, // 18
			 -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f, // 19

			 // Нижняя грань (Y-)
			 -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f, // 20
			  0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f, // 21
			  0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f, // 22
			 -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f, // 23
		};

		uint32_t inds[36] = {
			// Передняя грань (CCW)
			0, 1, 2,
			2, 3, 0,
			// Задняя грань (CCW)
			4, 6, 5,
			4, 7, 6,
			// Левая грань (CCW)
			8, 10, 9,
			8, 11, 10,
			// Правая грань (CCW)
			13, 14, 12,
			14, 15, 12,
			// Верхняя грань (CCW)
			16, 18, 17,
			16, 19, 18,
			// Нижняя грань (CCW)
			21, 22, 20,
			22, 23, 20,
		};

		MeshData data;
		data.verts = verts;
		data.vertSize = sizeof(verts);
		data.inds = inds;
		data.indsSize = sizeof(inds) / sizeof(uint32_t);

		Ref<Mesh> mesh = CreateRef<Mesh>(layout, data);

		AssetHandle handle;
		auto& path = ConstructPrimitivePath(PrimitiveType::Cube);

		MeshSerializer::Serialize(handle, mesh, data, path);

		return mesh;
	}
}