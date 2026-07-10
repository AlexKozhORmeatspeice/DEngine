#include "DEngine.h"
#include "DEngine/EntryPoint.h"

#include "imgui/imgui.h"

using namespace DEngine;

class ExampleLayer : public DEngine::Layer
{
public:
    ExampleLayer() 
        : DEngine::Layer("Example"), m_CamPos(0.0f), m_SquarePos(0.0f), m_CamRot(0.0f)
    {
    }

	virtual void Init() override 
    {
    }

    virtual void OnUpdate(const DEngine::Timestep& ts) override
    {
    }

	virtual void OnRender(const Timestep& ts) override
    {
    }

	virtual void OnImGuiRenderer() override
    {
    }

	virtual void OnEvent(DEngine::Event& event) override
    {
    }

    bool OnKeyPressedEv(DEngine::KeyPressedEvent& event)
    {
        return false;
    }
private:
    DEngine::Ref<DEngine::Camera> m_Camera;

	Ref<Mesh> m_CubeMesh;
	Ref<Material> m_Material;

	ShaderLibrary shaderLib;
	Ref<Texture2D> m_Texture;

	Ref<Scene> m_Scene;
    std::shared_ptr<BaseAssetManager> m_AssetManager;

    std::vector<DEngine::ProfileResult> m_ProfileResults;

    glm::vec3 m_CamPos;
    glm::vec3 m_CamRot;
    float m_CamSpeed = 2.0f;
    float m_CamRotSpeed = 100.0f;

    glm::vec3 m_SquarePos;
};

class Editor : public DEngine::Application
{
public:
    Editor() : DEngine::Application() 
    {
        PushLayer(new ExampleLayer());
    }

    virtual ~Editor() {}
};

DEngine::Application* DEngine::CreateApplication()
{
	return new Editor();
}
