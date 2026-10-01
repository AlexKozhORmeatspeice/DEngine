#include "dpch.h"
#include "MeshGenerator.h"

#include "DEngine/Core.h"
#include "DEngine/Asset/Serializer/MeshSerializer.h"
#include "DEngine/Project/Project.h"

#include <vector>
#include <cmath>

namespace DEngine
{
	const std::string PRIMITIVES_DIR_NAME = "primitives";

	Ref<Mesh> MeshGenerator::CreatePrimitive(PrimitiveType type)
	{
		switch (type)
		{
		case DEngine::PrimitiveType::Cube:   return CreateCube();
		case DEngine::PrimitiveType::Sphere: return CreateSphere();
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
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_Texcoord" },
			{ ShaderDataType::Float3, "a_Tangent" },
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

		auto& path = ConstructPrimitivePath(PrimitiveType::Cube);
		MeshSerializer::Serialize(mesh, data, path);

		return mesh;
	}

	Ref<Mesh> MeshGenerator::CreateSphere()
	{
		BufferLayout layout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_Texcoord" },
			{ ShaderDataType::Float3, "a_Tangent" },
		};

		constexpr uint32_t stacks = 16;  // параллели (от полюса до полюса)
		constexpr uint32_t slices = 16;   // меридианы
		constexpr float radius = 0.5f;
		constexpr float PI = 3.14159265358979323846f;

		const uint32_t vertsPerStack = slices + 1;                 // +1 для дублирования шва UV
		const uint32_t vertCount = (stacks + 1) * vertsPerStack;
		const uint32_t indCount = stacks * slices * 6;

		std::vector<float> verts;
		verts.reserve(vertCount * 11);

		// Генерация вершин
		for (uint32_t i = 0; i <= stacks; ++i)
		{
			// phi: 0 (верхний полюс, +Y) .. PI (нижний полюс, -Y)
			float phi = PI * static_cast<float>(i) / static_cast<float>(stacks);
			float sinPhi = std::sin(phi);
			float cosPhi = std::cos(phi);

			for (uint32_t j = 0; j <= slices; ++j)
			{
				// theta: 0 .. 2PI
				float theta = 2.0f * PI * static_cast<float>(j) / static_cast<float>(slices);
				float sinTheta = std::sin(theta);
				float cosTheta = std::cos(theta);

				// Позиция на сфере
				float x = radius * sinPhi * cosTheta;
				float y = radius * cosPhi;
				float z = radius * sinPhi * sinTheta;

				// Нормаль (для сферы совпадает с нормализованной позицией)
				float nx = sinPhi * cosTheta;
				float ny = cosPhi;
				float nz = sinPhi * sinTheta;

				// UV
				float u = static_cast<float>(j) / static_cast<float>(slices);
				float v = 1.0f - static_cast<float>(i) / static_cast<float>(stacks);

				// Тангент: производная позиции по theta
				float tx = -sinTheta;
				float ty = 0.0f;
				float tz = cosTheta;

				verts.push_back(x);  verts.push_back(y);  verts.push_back(z);
				verts.push_back(nx); verts.push_back(ny); verts.push_back(nz);
				verts.push_back(u);  verts.push_back(v);
				verts.push_back(tx); verts.push_back(ty); verts.push_back(tz);
			}
		}

		// Генерация индексов (CCW)
		std::vector<uint32_t> inds;
		inds.reserve(indCount);

		for (uint32_t i = 0; i < stacks; ++i)
		{
			for (uint32_t j = 0; j < slices; ++j)
			{
				uint32_t a = i * vertsPerStack + j;         // текущая параллель, текущий меридиан
				uint32_t b = a + 1;                          // текущая параллель, следующий меридиан
				uint32_t c = (i + 1) * vertsPerStack + j;   // следующая параллель, текущий меридиан
				uint32_t d = c + 1;                          // следующая параллель, следующий меридиан

				inds.push_back(a); inds.push_back(c); inds.push_back(b);
				inds.push_back(b); inds.push_back(c); inds.push_back(d);
			}
		}

		MeshData data;
		data.verts = verts.data();
		data.vertSize = verts.size() * sizeof(float);
		data.inds = inds.data();
		data.indsSize = inds.size();

		Ref<Mesh> mesh = CreateRef<Mesh>(layout, data);

		auto& path = ConstructPrimitivePath(PrimitiveType::Sphere);
		MeshSerializer::Serialize(mesh, data, path);

		return mesh;
	}
}