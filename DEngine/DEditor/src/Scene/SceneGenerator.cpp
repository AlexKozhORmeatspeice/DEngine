#include "SceneGenerator.h"

#include <string>

namespace DEngine
{
	const char* SceneGenerator::BaseScenePath()
	{
		return "assets/scenes/Example.dscene";
	}

	Ref<Scene> SceneGenerator::Create()
	{
		Ref<Scene> scene = CreateRef<Scene>();

		const AssetHandle texHandle = AssetManager::CreateAsset("assets/textures/pasha.jpg");

		Ref<Material> mat = CreateRef<Material>(AssetManager::GetBaseRendererShaderHandle());
		mat->SetTexture2D("u_Texture", texHandle);

		Ref<Material> mat2 = CreateRef<Material>(AssetManager::GetBaseRendererShaderHandle());

		std::string matName = "basemat";
		std::string matName2 = "basemat2";
		matName += AssetHelper::NativeExtension(AssetType::Material);
		matName2 += AssetHelper::NativeExtension(AssetType::Material);
		const AssetHandle matHandle = AssetManager::CreateMaterialAsset(mat, "assets/materials/" + matName);
		const AssetHandle matHandle2 = AssetManager::CreateMaterialAsset(mat2, "assets/materials/" + matName2);

		const AssetHandle sponzaHandle = AssetManager::CreateAsset({ AssetType::Model, "assets/models/sponza.obj-master/sponza.obj" });
		const AssetHandle meshHandle = AssetManager::GetPrimitiveMeshHandle(PrimitiveType::Cube);
		const AssetHandle sphereMeshHandle = AssetManager::GetPrimitiveMeshHandle(PrimitiveType::Sphere);

		auto& cube = scene->CreateEntity("cubee");
		cube.AddComponent<MeshRendererComponent>(meshHandle, matHandle);
		cube.AddComponent<RigidbodyComponent>().isKinematic = true;
		cube.AddComponent<ColliderComponent>();

		auto& trans = cube.GetComponent<TransformComponent>();
		trans.SetScale({ 100.0f, 100.0f, 100.0f });
		trans.SetPosition({ 0.0f, 0.0f, -200.0f });

		auto& cube2 = scene->CreateEntity("cube2");
		cube2.AddComponent<MeshRendererComponent>(meshHandle, matHandle2);
		cube2.AddComponent<RigidbodyComponent>().isKinematic = true;
		cube2.AddComponent<ColliderComponent>();

		auto& trans2 = cube2.GetComponent<TransformComponent>();
		trans2.SetScale({ 400.0f, 50.0f, 400.0f });
		trans2.SetPosition({ 0.0f, -100.0f, 0.0f });

		auto& cube3 = scene->CreateEntity("cube3");
		cube3.AddComponent<MeshRendererComponent>(meshHandle, matHandle);
		cube3.AddComponent<RigidbodyComponent>().isKinematic = true;
		cube3.AddComponent<ColliderComponent>();

		auto& trans3 = cube3.GetComponent<TransformComponent>();
		trans3.SetScale({ 100.0f, 100.0f, 100.0f });
		trans3.SetPosition({ 0.0f, 500.0f, -200.0f });

		auto& sphere1 = scene->CreateEntity("sphere1");
		sphere1.AddComponent<MeshRendererComponent>(sphereMeshHandle, matHandle);
		sphere1.AddComponent<RigidbodyComponent>().isKinematic = true;
		sphere1.AddComponent<ColliderComponent>();

		auto& sphere1trans = sphere1.GetComponent<TransformComponent>();
		sphere1trans.SetScale({ 100.0f, 100.0f, 100.0f });
		sphere1trans.SetPosition({ 50.0f, 200.0f, -100.0f });

		auto& sphere2 = scene->CreateEntity("sphere2");
		sphere2.AddComponent<MeshRendererComponent>(sphereMeshHandle, matHandle);
		sphere2.AddComponent<RigidbodyComponent>().isKinematic = true;
		sphere2.AddComponent<ColliderComponent>();

		auto& sphere2trans = sphere2.GetComponent<TransformComponent>();
		sphere2trans.SetScale({ 100.0f, 100.0f, 100.0f });
		sphere2trans.SetPosition({ 50.0f, 400.0f, -100.0f });

		auto& directLight = scene->CreateEntity("direct light");
		directLight.AddComponent<DirectLightComponent>(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
		auto& lightTrans = directLight.GetComponent<TransformComponent>();
		lightTrans.Rotate(70.0f, { 0.0f, 1.0f, 0.0f });

		auto& mainCamera = scene->CreateEntity("Main Camera");
		mainCamera.AddComponent<CameraComponent>();
		auto& cameraTrans = mainCamera.GetComponent<TransformComponent>();
		cameraTrans.SetPosition({ 0.0f, 150.0f, 250.0f });

		Entity sponzaRoot = scene->CreateEntity("Sponza");
		uint32_t partIndex = 0;
		for (const auto& renderData : AssetManager::GetAsset<Model>(sponzaHandle)->GetRenderData())
		{
			Entity part = scene->CreateEntity("Sponza " + std::to_string(partIndex++));
			part.AddComponent<MeshRendererComponent>(renderData.MeshHandle, renderData.second);
			scene->SetParent(part, sponzaRoot);
		}

		AssetManager::CreateSceneAsset(scene, BaseScenePath());
		return scene;
	}
}
