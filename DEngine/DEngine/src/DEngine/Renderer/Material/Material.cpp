#include "dpch.h"
#include "Material.h"

#include "DEngine/Asset/AssetManager.h"

namespace DEngine
{
	Material::Material(const AssetHandle& shaderHandle) : m_ShaderHandle(shaderHandle) {}

	void Material::SetInt(const std::string& name, int val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_IntValues.find(id);
		m_IntValues[id] = val;
		m_DirtyInts.push_back(id);
	}

	void Material::SetFloat(const std::string& name, float val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_FloatValues.find(id);
		m_FloatValues[id] = val;
		m_DirtyFloats.push_back(id);
	}

	void Material::SetFloat2(const std::string& name, const glm::vec2& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_Vec2Values.find(id);
		m_Vec2Values[id] = val;
		m_DirtyVec2s.push_back(id);
	}

	void Material::SetFloat3(const std::string& name, const glm::vec3& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_Vec3Values.find(id);
		m_Vec3Values[id] = val;
		m_DirtyVec3s.push_back(id);
	}

	void Material::SetFloat4(const std::string& name, const glm::vec4& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_Vec4Values.find(id);
		m_Vec4Values[id] = val;
		m_DirtyVec4s.push_back(id);
	}

	void Material::SetMat3(const std::string& name, const glm::mat3& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_Mat3Values.find(id);
		m_Mat3Values[id] = val;
		m_DirtyMat3s.push_back(id);
	}

	void Material::SetMat4(const std::string& name, const glm::mat4& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		m_Mat4Values[id] = val;
		m_DirtyMat4s.push_back(id);
	}

	void Material::SetTexture2D(const std::string& name, const AssetHandle& tex)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_TextureValues.find(id);
		m_TextureValues[id] = tex;
		m_DirtyTextures.push_back(id);
	}

	void Material::Bind()
	{
		AssetManager::GetAsset<Shader>(m_ShaderHandle)->Bind();

		UpdateDirtyValues();

		for (const auto& [id, textureHandle] : m_TextureValues)
		{
			auto texture = AssetManager::GetAsset<Texture2D>(textureHandle);
			texture->Bind();
		}
	}

	void Material::Unbind()
	{
		AssetManager::GetAsset<Shader>(m_ShaderHandle)->Unbind();
	}

	void Material::UpdateDirtyValues()
	{
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(m_ShaderHandle);

		for (uint32_t id : m_DirtyInts)
			shader->UploadUniformInt(id, m_IntValues[id]);
		m_DirtyInts.clear();

		for (uint32_t id : m_DirtyFloats)
			shader->UploadUniformFloat(id, m_FloatValues[id]);
		m_DirtyFloats.clear();

		for (uint32_t id : m_DirtyVec2s)
			shader->UploadUniformFloat2(id, m_Vec2Values[id]);
		m_DirtyVec2s.clear();

		for (uint32_t id : m_DirtyVec3s)
			shader->UploadUniformFloat3(id, m_Vec3Values[id]);
		m_DirtyVec3s.clear();

		for (uint32_t id : m_DirtyVec4s)
			shader->UploadUniformFloat4(id, m_Vec4Values[id]);
		m_DirtyVec4s.clear();

		for (uint32_t id : m_DirtyMat3s)
			shader->UploadUniformMat3(id, m_Mat3Values[id]);
		m_DirtyMat3s.clear();

		for (uint32_t id : m_DirtyMat4s)
			shader->UploadUniformMat4(id, m_Mat4Values[id]);
		m_DirtyMat4s.clear();

		for (uint32_t id : m_DirtyTextures)
			shader->UploadUniformInt(id, 0); //TODO: надо какую-то систему слотов для текстур сделать вероятно
		m_DirtyTextures.clear();
	}

	uint32_t Material::GetCachedID(const std::string& name)
	{
		auto it = m_NameToID.find(name);
		if (it != m_NameToID.end())
			return it->second;
		
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(m_ShaderHandle);
		uint32_t id = shader->GetUniformLocation(name);

		if (id != UINT32_MAX)
			m_NameToID[name] = id;
		
		return id;
	}
}