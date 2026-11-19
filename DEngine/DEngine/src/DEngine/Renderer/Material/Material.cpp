#include "dpch.h"
#include "Material.h"

#include "DEngine/Asset/AssetManager.h"
#include "DEngine/Renderer/Renderer.h"
#include "DEngine/Renderer/Pipeline/DescriptorSet.h"

#include "DEngine/Profiler/Profiler.h"

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
		m_NamedTextures[name] = tex;
		m_WrittenTextures.clear();
		GetTexture(tex);

		uint32_t id = GetCachedID(name);
		if (id == UINT32_MAX)
			return;

		m_TextureValues[id] = tex;
		m_DirtyTextures.push_back(id);
	}

	void Material::Bind(CommandBuffer& commandBuffer)
	{
		D_PROFILE_FUNCTION();
		Ref<Shader> shader = GetShader();
		if (!shader)
		{
			D_CORE_ERROR("Material::Bind: shader {0} is not loaded", static_cast<uint64_t>(m_ShaderHandle));
			return;
		}

		shader->Bind(commandBuffer);

		if (Ref<DescriptorSet> textureSet = EnsureTextureSet())
			textureSet->Bind(commandBuffer, Renderer::GetPipelineLayout());
	}

	void Material::PrepareForRender()
	{
		EnsureTextureSet();
	}

	Ref<DescriptorSet> Material::EnsureTextureSet()
	{
		if (!GetShader())
			return nullptr;

		std::lock_guard<std::mutex> lock(m_BindMutex);
		UpdateDirtyValues();

		const uint32_t image = Renderer::GetFrameInFlightIndex();
		if (m_TextureSets.size() <= image)
			m_TextureSets.resize(image + 1);
		if (!m_TextureSets[image])
			m_TextureSets[image] = DescriptorSet::Create(Renderer::GetPipelineLayout(), 1);

		if (m_WrittenTextures.size() <= image)
			m_WrittenTextures.resize(image + 1);

		if (Ref<Texture> texture = GetTexture(ResolveTextureHandle()))
		{
			if (m_WrittenTextures[image] != texture)
			{
				m_TextureSets[image]->WriteTexture(0, texture);
				m_WrittenTextures[image] = texture;
			}
		}

		return m_TextureSets[image];
	}

	void Material::Unbind(CommandBuffer& commandBuffer)
	{
		if (m_Shader)
			m_Shader->Unbind(commandBuffer);
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

	namespace
	{
		template<typename T>
		std::unordered_map<uint32_t, T> RemapUniformValues(const std::unordered_map<uint32_t, T>& values,
														  const std::unordered_map<uint32_t, uint32_t>& oldToNew)
		{
			std::unordered_map<uint32_t, T> remapped;
			for (const auto& [oldId, value] : values)
			{
				const auto found = oldToNew.find(oldId);
				if (found == oldToNew.end() || found->second == UINT32_MAX)
					continue;
				remapped.emplace(found->second, value);
			}
			return remapped;
		}

		template<typename T>
		void MarkDirty(const std::unordered_map<uint32_t, T>& values, std::vector<uint32_t>& dirty)
		{
			dirty.clear();
			dirty.reserve(values.size());
			for (const auto& [id, _] : values)
				dirty.push_back(id);
		}
	}

	void Material::InvalidateCachedShader()
	{
		m_Shader = nullptr;
		Ref<Shader> shader = GetShader();
		if (!shader)
			return;

		std::unordered_map<uint32_t, uint32_t> oldToNew;
		std::unordered_map<std::string, uint32_t> names;
		for (const auto& [name, oldId] : m_NameToID)
		{
			const uint32_t newId = shader->GetUniformLocation(name);
			oldToNew.emplace(oldId, newId);
			if (newId != UINT32_MAX)
				names.emplace(name, newId);
		}

		m_IntValues = RemapUniformValues(m_IntValues, oldToNew);
		m_FloatValues = RemapUniformValues(m_FloatValues, oldToNew);
		m_Vec2Values = RemapUniformValues(m_Vec2Values, oldToNew);
		m_Vec3Values = RemapUniformValues(m_Vec3Values, oldToNew);
		m_Vec4Values = RemapUniformValues(m_Vec4Values, oldToNew);
		m_Mat3Values = RemapUniformValues(m_Mat3Values, oldToNew);
		m_Mat4Values = RemapUniformValues(m_Mat4Values, oldToNew);
		m_TextureValues = RemapUniformValues(m_TextureValues, oldToNew);
		m_NameToID = std::move(names);

		MarkDirty(m_IntValues, m_DirtyInts);
		MarkDirty(m_FloatValues, m_DirtyFloats);
		MarkDirty(m_Vec2Values, m_DirtyVec2s);
		MarkDirty(m_Vec3Values, m_DirtyVec3s);
		MarkDirty(m_Vec4Values, m_DirtyVec4s);
		MarkDirty(m_Mat3Values, m_DirtyMat3s);
		MarkDirty(m_Mat4Values, m_DirtyMat4s);
		MarkDirty(m_TextureValues, m_DirtyTextures);
	}

	void Material::InvalidateCachedTexture(const AssetHandle& handle)
	{
		m_Textures.erase(handle);
		for (Ref<Texture>& written : m_WrittenTextures)
		{
			if (written && written->handle == handle)
				written = nullptr;
		}
	}

	AssetHandle Material::ResolveTextureHandle() const
	{
		auto named = m_NamedTextures.find("u_Texture");
		if (named != m_NamedTextures.end())
			return named->second;

		auto id = m_NameToID.find("u_Texture");
		if (id != m_NameToID.end())
		{
			auto value = m_TextureValues.find(id->second);
			if (value != m_TextureValues.end())
				return value->second;
		}

		if (!m_TextureValues.empty())
			return m_TextureValues.begin()->second;

		return AssetManager::GetEmptyTextureHandle();
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
