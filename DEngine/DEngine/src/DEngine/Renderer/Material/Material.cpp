#include "dpch.h"
#include "Material.h"

namespace DEngine
{

	Material::Material(Ref<Shader> shader) : m_Shader(shader) {}

	void Material::SetInt(const std::string& name, int val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_IntValues.find(id);
		if (it == m_IntValues.end() || it->second != val)
		{
			m_IntValues[id] = val;
			m_DirtyInts.push_back(id);
		}
	}

	void Material::SetFloat(const std::string& name, float val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_FloatValues.find(id);
		if (it == m_FloatValues.end() || it->second != val)
		{
			m_FloatValues[id] = val;
			m_DirtyFloats.push_back(id);
		}
	}

	void Material::SetFloat2(const std::string& name, const glm::vec2& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_Vec2Values.find(id);
		if (it == m_Vec2Values.end() || it->second != val)
		{
			m_Vec2Values[id] = val;
			m_DirtyVec2s.push_back(id);
		}
	}

	void Material::SetFloat3(const std::string& name, const glm::vec3& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_Vec3Values.find(id);
		if (it == m_Vec3Values.end() || it->second != val)
		{
			m_Vec3Values[id] = val;
			m_DirtyVec3s.push_back(id);
		}
	}

	void Material::SetFloat4(const std::string& name, const glm::vec4& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_Vec4Values.find(id);
		if (it == m_Vec4Values.end() || it->second != val)
		{
			m_Vec4Values[id] = val;
			m_DirtyVec4s.push_back(id);
		}
	}

	void Material::SetMat3(const std::string& name, const glm::mat3& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_Mat3Values.find(id);
		if (it == m_Mat3Values.end() || it->second != val)
		{
			m_Mat3Values[id] = val;
			m_DirtyMat3s.push_back(id);
		}
	}

	void Material::SetMat4(const std::string& name, const glm::mat4& val)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_Mat4Values.find(id);
		if (it == m_Mat4Values.end() || it->second != val)
		{
			m_Mat4Values[id] = val;
			m_DirtyMat4s.push_back(id);
		}
	}

	void Material::SetTexture2D(const std::string& name, const Ref<Texture2D>& tex)
	{
		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX) return;
		
		auto it = m_TextureValues.find(id);
		if (it == m_TextureValues.end() || it->second != tex)
		{
			m_TextureValues[id] = tex;
			m_DirtyTextures.push_back(id);
		}
	}

	void Material::Bind()
	{
		UpdateDirtyValues();

		for (const auto& [id, texture] : m_TextureValues)
		{
			texture->Bind();
		}
		m_Shader->Bind();
	}

	void Material::Unbind()
	{
		m_Shader->Unbind();
	}

	void Material::UpdateDirtyValues()
	{
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
		
		uint32_t id = m_Shader->GetUniformLocation(name);
		if (id != UINT32_MAX)
			m_NameToID[name] = id;
		return id;
	}
}