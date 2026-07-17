#pragma once

#include "DEngine/Core.h"
#include "DEngine/Renderer/Shader/Shader.h"
#include "DEngine/Renderer/Texture/Texture.h"

#include "DEngine/Asset/Asset.h"

#include <unordered_map>
#include <vector>
#include <cstring>

namespace DEngine
{
    class Material : public Asset
    {
    public:
        Material(const AssetHandle& shaderHandle);

        void SetInt(const std::string& name, int val);
        void SetFloat(const std::string& name, float val);
        void SetFloat2(const std::string& name, const glm::vec2& val);
        void SetFloat3(const std::string& name, const glm::vec3& val);
        void SetFloat4(const std::string& name, const glm::vec4& val);
        void SetMat3(const std::string& name, const glm::mat3& val);
        void SetMat4(const std::string& name, const glm::mat4& val);
        void SetTexture2D(const std::string& name, const AssetHandle& tex);

        void Bind();
        void Unbind();

		static AssetType GetStaticType() { return AssetType::Material; }
		virtual AssetType GetType() const override { return GetStaticType(); }

    private:
        void UpdateDirtyValues();

        uint32_t GetCachedID(const std::string& name);

    private:
        AssetHandle m_ShaderHandle;
        
        std::unordered_map<std::string, uint32_t> m_NameToID;
        
        std::unordered_map<uint32_t, int> m_IntValues;
        std::unordered_map<uint32_t, float> m_FloatValues;
        std::unordered_map<uint32_t, glm::vec2> m_Vec2Values;
        std::unordered_map<uint32_t, glm::vec3> m_Vec3Values;
        std::unordered_map<uint32_t, glm::vec4> m_Vec4Values;
        std::unordered_map<uint32_t, glm::mat3> m_Mat3Values;
        std::unordered_map<uint32_t, glm::mat4> m_Mat4Values;
        std::unordered_map<uint32_t, AssetHandle> m_TextureValues;
        
        std::vector<uint32_t> m_DirtyInts;
        std::vector<uint32_t> m_DirtyFloats;
        std::vector<uint32_t> m_DirtyVec2s;
        std::vector<uint32_t> m_DirtyVec3s;
        std::vector<uint32_t> m_DirtyVec4s;
        std::vector<uint32_t> m_DirtyMat3s;
        std::vector<uint32_t> m_DirtyMat4s;
        std::vector<uint32_t> m_DirtyTextures;

        friend class MaterialSerializer;
    };
}