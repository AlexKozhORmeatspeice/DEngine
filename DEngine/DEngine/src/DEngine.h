#pragma once

// For use by DEngine applictions
#include "DEngine/Core.h"
#include "DEngine/Application.h"
#include "DEngine/DLog.h"
#include "DEngine/Layer/Layer.h"
#include "DEngine/Input/Input.h"
#include "DEngine/ImGui/ImGuiLayer.h"

#include "DEngine/Input/KeyCodes.h"
#include "DEngine/Input/MouseButtonCodes.h"

#include "DEngine/Core/Timestep.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "DEngine/Window.h"

#include "DEngine/Scene/Scene.h"
#include "DEngine/Scene/Entity.h"
#include "DEngine/Scene/Components.h"
#include "DEngine/Scene/Systems.h"

#include "DEngine/Project/Project.h"

//--Assets--
#include "DEngine/Asset/Asset.h"
#include "DEngine/Asset/AssetManagerBase.h"
#include "DEngine/Asset/EditorAssetManager.h"
#include "DEngine/Asset/RuntimeAssetManager.h"
#include "DEngine/Asset/AssetManager.h"
//---------

//--Renderer--
#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Renderer/RenderCommand.h"
#include "DEngine/Renderer/GraphicsContext.h"
#include "DEngine/Renderer/RendererAPI.h"
#include "DEngine/Renderer/Texture.h"
#include "DEngine/Renderer/Shader/Shader.h"
#include "DEngine/Renderer/Buffers/Buffer.h"
#include "DEngine/Renderer/Buffers/Framebuffer.h"
#include "DEngine/Renderer/Buffers/VertexArray.h"
#include "DEngine/Renderer/Mesh/Mesh.h"
#include "DEngine/Renderer/Mesh/MeshGenerator.h"
#include "DEngine/Renderer/Material/Material.h"
#include "DEngine/Renderer/Cameras/Camera.h"
#include "DEngine/Renderer/Cameras/OrthCamera.h"
#include "DEngine/Renderer/Cameras/PerspCamera.h"
#include "DEngine/Renderer/Texture/Texture.h"
//------------

//--Utils--
#include "DEngine/Utils/Timer.h"
//---------
