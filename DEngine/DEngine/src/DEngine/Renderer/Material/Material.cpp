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

		m_IntValues[id] = val;
		m_DirtyInts.push_back(id);
	}

	void Material::SetFloat(const std::string& name, float val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;

		m_FloatValues[id] = val;
		m_DirtyFloats.push_back(id);
	}

	void Material::SetFloat2(const std::string& name, const glm::vec2& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;

		m_Vec2Values[id] = val;
		m_DirtyVec2s.push_back(id);
	}

	void Material::SetFloat3(const std::string& name, const glm::vec3& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;

		m_Vec3Values[id] = val;
		m_DirtyVec3s.push_back(id);
	}

	void Material::SetFloat4(const std::string& name, const glm::vec4& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;

		m_Vec4Values[id] = val;
		m_DirtyVec4s.push_back(id);
	}

	void Material::SetMat3(const std::string& name, const glm::mat3& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;

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

		m_TextureValues[id] = tex;
		GetTexture(tex);
		m_DirtyTextures.push_back(id);
	}

	void Material::Bind()
	{
		Ref<Shader> shader = GetShader();
		if (!shader)
		{
			D_CORE_ERROR("Material::Bind: shader {0} is not loaded", static_cast<uint64_t>(m_ShaderHandle));
			return;
		}

		shader->Bind();
		UpdateDirtyValues();

		if (m_TextureValues.empty())
		{
			if (Ref<Texture> texture = GetTexture(AssetManager::GetEmptyTextureHandle()))
				texture->Bind();
			return;
		}

		for (const auto& [id, textureHandle] : m_TextureValues)
		{
			if (Ref<Texture> texture = GetTexture(textureHandle))
				texture->Bind();
		}
	}

	void Material::Unbind()
	{
		if (m_Shader)
			m_Shader->Unbind();
	}

	void Material::UpdateDirtyValues()
	{
		if (!m_Shader)
			return;

		for (uint32_t id : m_DirtyInts)
			m_Shader->UploadUniformInt(id, m_IntValues[id]);
		m_DirtyInts.clear();

		for (uint32_t id : m_DirtyFloats)
			m_Shader->UploadUniformFloat(id, m_FloatValues[id]);
		m_DirtyFloats.clear();

		for (uint32_t id : m_DirtyVec2s)
			m_Shader->UploadUniformFloat2(id, m_Vec2Values[id]);
		m_DirtyVec2s.clear();

		for (uint32_t id : m_DirtyVec3s)
			m_Shader->UploadUniformFloat3(id, m_Vec3Values[id]);
		m_DirtyVec3s.clear();

		for (uint32_t id : m_DirtyVec4s)
			m_Shader->UploadUniformFloat4(id, m_Vec4Values[id]);
		m_DirtyVec4s.clear();

		for (uint32_t id : m_DirtyMat3s)
			m_Shader->UploadUniformMat3(id, m_Mat3Values[id]);
		m_DirtyMat3s.clear();

		for (uint32_t id : m_DirtyMat4s)
			m_Shader->UploadUniformMat4(id, m_Mat4Values[id]);
		m_DirtyMat4s.clear();

		for (uint32_t id : m_DirtyTextures)
			m_Shader->UploadUniformInt(id, 0); //TODO: надо какую-то систему слотов для текстур сделать вероятно
		m_DirtyTextures.clear();
	}

	uint32_t Material::GetCachedID(const std::string& name)
	{
		auto it = m_NameToID.find(name);
		if (it != m_NameToID.end())
			return it->second;

		Ref<Shader> shader = GetShader();
		if (!shader)
			return UINT32_MAX;

		uint32_t id = shader->GetUniformLocation(name);
		if (id != UINT32_MAX)
			m_NameToID[name] = id;

		return id;
	}

	Ref<Shader> Material::GetShader()
	{
		if (!m_Shader)
			m_Shader = AssetManager::GetAsset<Shader>(m_ShaderHandle);
		return m_Shader;
	}

	Ref<Texture> Material::GetTexture(const AssetHandle& handle)
	{
		auto found = m_Textures.find(handle);
		if (found != m_Textures.end())
			return found->second;

		Ref<Texture> texture = AssetManager::GetAsset<Texture2D>(handle);
		const AssetHandle& emptyHandle = AssetManager::GetEmptyTextureHandle();
		const bool stillLoading = texture && handle != emptyHandle && texture->handle == emptyHandle;
		if (stillLoading)
			return texture;

		if (!texture && handle != emptyHandle)
			texture = GetTexture(emptyHandle);

		if (texture)
			m_Textures.emplace(handle, texture);
		return texture;
	}
}
