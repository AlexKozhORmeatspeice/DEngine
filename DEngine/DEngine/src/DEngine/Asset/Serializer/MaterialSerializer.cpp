#include "dpch.h"
#include "MaterialSerializer.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace DEngine
{
    // Вспомогательная функция для записи строки
    static void WriteString(std::ofstream& stream, const std::string& str)
    {
        uint32_t length = static_cast<uint32_t>(str.length());
        stream.write(reinterpret_cast<const char*>(&length), sizeof(length));
        stream.write(str.c_str(), length);
    }

    // Вспомогательная функция для чтения строки
    static std::string ReadString(std::ifstream& stream)
    {
        uint32_t length;
        stream.read(reinterpret_cast<char*>(&length), sizeof(length));
        std::string str(length, '\0');
        stream.read(&str[0], length);
        return str;
    }

    // Вспомогательная функция для записи вектора
    template<typename T>
    static void WriteVector(std::ofstream& stream, const std::vector<T>& vec)
    {
        uint32_t size = static_cast<uint32_t>(vec.size());
        stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
        if constexpr (std::is_trivially_copyable_v<T>)
        {
            stream.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(T));
        }
        else
        {
            for (const auto& item : vec)
            {
                stream.write(reinterpret_cast<const char*>(&item), sizeof(item));
            }
        }
    }

    // Вспомогательная функция для чтения вектора тривиально копируемых типов
    template<typename T>
    static std::vector<T> ReadVector(std::ifstream& stream)
    {
        uint32_t size;
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::vector<T> vec(size);
        if constexpr (std::is_trivially_copyable_v<T>)
        {
            stream.read(reinterpret_cast<char*>(vec.data()), size * sizeof(T));
        }
        else
        {
            for (auto& item : vec)
            {
                stream.read(reinterpret_cast<char*>(&item), sizeof(item));
            }
        }
        return vec;
    }

    // Функция для записи unordered_map с тривиально копируемыми типами
    template<typename K, typename V>
    static void WriteMapSimple(std::ofstream& stream, const std::unordered_map<K, V>& map)
    {
        uint32_t size = static_cast<uint32_t>(map.size());
        stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& [key, value] : map)
        {
            stream.write(reinterpret_cast<const char*>(&key), sizeof(key));
            stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }
    }

    // Функция для чтения unordered_map с тривиально копируемыми типами
    template<typename K, typename V>
    static std::unordered_map<K, V> ReadMapSimple(std::ifstream& stream)
    {
        uint32_t size;
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::unordered_map<K, V> map;
        map.reserve(size);
        for (uint32_t i = 0; i < size; ++i)
        {
            K key;
            V value;
            stream.read(reinterpret_cast<char*>(&key), sizeof(key));
            stream.read(reinterpret_cast<char*>(&value), sizeof(value));
            map[key] = value;
        }
        return map;
    }

    // Специализация для записи map с ключом std::string
    static void WriteNameToIDMap(std::ofstream& stream, const std::unordered_map<std::string, uint32_t>& map)
    {
        uint32_t size = static_cast<uint32_t>(map.size());
        stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& [key, value] : map)
        {
            WriteString(stream, key);
            stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }
    }

    // Специализация для чтения map с ключом std::string
    static std::unordered_map<std::string, uint32_t> ReadNameToIDMap(std::ifstream& stream)
    {
        uint32_t size;
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::unordered_map<std::string, uint32_t> map;
        map.reserve(size);
        for (uint32_t i = 0; i < size; ++i)
        {
            std::string key = ReadString(stream);
            uint32_t value;
            stream.read(reinterpret_cast<char*>(&value), sizeof(value));
            map[key] = value;
        }
        return map;
    }

    // Специализации для glm типов
    static void WriteVec2Map(std::ofstream& stream, const std::unordered_map<uint32_t, glm::vec2>& map)
    {
        uint32_t size = static_cast<uint32_t>(map.size());
        stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& [key, value] : map)
        {
            stream.write(reinterpret_cast<const char*>(&key), sizeof(key));
            stream.write(reinterpret_cast<const char*>(glm::value_ptr(value)), sizeof(glm::vec2));
        }
    }

    static std::unordered_map<uint32_t, glm::vec2> ReadVec2Map(std::ifstream& stream)
    {
        uint32_t size;
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::unordered_map<uint32_t, glm::vec2> map;
        map.reserve(size);
        for (uint32_t i = 0; i < size; ++i)
        {
            uint32_t key;
            glm::vec2 value;
            stream.read(reinterpret_cast<char*>(&key), sizeof(key));
            stream.read(reinterpret_cast<char*>(glm::value_ptr(value)), sizeof(glm::vec2));
            map[key] = value;
        }
        return map;
    }

    static void WriteVec3Map(std::ofstream& stream, const std::unordered_map<uint32_t, glm::vec3>& map)
    {
        uint32_t size = static_cast<uint32_t>(map.size());
        stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& [key, value] : map)
        {
            stream.write(reinterpret_cast<const char*>(&key), sizeof(key));
            stream.write(reinterpret_cast<const char*>(glm::value_ptr(value)), sizeof(glm::vec3));
        }
    }

    static std::unordered_map<uint32_t, glm::vec3> ReadVec3Map(std::ifstream& stream)
    {
        uint32_t size;
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::unordered_map<uint32_t, glm::vec3> map;
        map.reserve(size);
        for (uint32_t i = 0; i < size; ++i)
        {
            uint32_t key;
            glm::vec3 value;
            stream.read(reinterpret_cast<char*>(&key), sizeof(key));
            stream.read(reinterpret_cast<char*>(glm::value_ptr(value)), sizeof(glm::vec3));
            map[key] = value;
        }
        return map;
    }

    static void WriteVec4Map(std::ofstream& stream, const std::unordered_map<uint32_t, glm::vec4>& map)
    {
        uint32_t size = static_cast<uint32_t>(map.size());
        stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& [key, value] : map)
        {
            stream.write(reinterpret_cast<const char*>(&key), sizeof(key));
            stream.write(reinterpret_cast<const char*>(glm::value_ptr(value)), sizeof(glm::vec4));
        }
    }

    static std::unordered_map<uint32_t, glm::vec4> ReadVec4Map(std::ifstream& stream)
    {
        uint32_t size;
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::unordered_map<uint32_t, glm::vec4> map;
        map.reserve(size);
        for (uint32_t i = 0; i < size; ++i)
        {
            uint32_t key;
            glm::vec4 value;
            stream.read(reinterpret_cast<char*>(&key), sizeof(key));
            stream.read(reinterpret_cast<char*>(glm::value_ptr(value)), sizeof(glm::vec4));
            map[key] = value;
        }
        return map;
    }

    static void WriteMat3Map(std::ofstream& stream, const std::unordered_map<uint32_t, glm::mat3>& map)
    {
        uint32_t size = static_cast<uint32_t>(map.size());
        stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& [key, value] : map)
        {
            stream.write(reinterpret_cast<const char*>(&key), sizeof(key));
            stream.write(reinterpret_cast<const char*>(glm::value_ptr(value)), sizeof(glm::mat3));
        }
    }

    static std::unordered_map<uint32_t, glm::mat3> ReadMat3Map(std::ifstream& stream)
    {
        uint32_t size;
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::unordered_map<uint32_t, glm::mat3> map;
        map.reserve(size);
        for (uint32_t i = 0; i < size; ++i)
        {
            uint32_t key;
            glm::mat3 value;
            stream.read(reinterpret_cast<char*>(&key), sizeof(key));
            stream.read(reinterpret_cast<char*>(glm::value_ptr(value)), sizeof(glm::mat3));
            map[key] = value;
        }
        return map;
    }

    static void WriteMat4Map(std::ofstream& stream, const std::unordered_map<uint32_t, glm::mat4>& map)
    {
        uint32_t size = static_cast<uint32_t>(map.size());
        stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& [key, value] : map)
        {
            stream.write(reinterpret_cast<const char*>(&key), sizeof(key));
            stream.write(reinterpret_cast<const char*>(glm::value_ptr(value)), sizeof(glm::mat4));
        }
    }

    static std::unordered_map<uint32_t, glm::mat4> ReadMat4Map(std::ifstream& stream)
    {
        uint32_t size;
        stream.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::unordered_map<uint32_t, glm::mat4> map;
        map.reserve(size);
        for (uint32_t i = 0; i < size; ++i)
        {
            uint32_t key;
            glm::mat4 value;
            stream.read(reinterpret_cast<char*>(&key), sizeof(key));
            stream.read(reinterpret_cast<char*>(glm::value_ptr(value)), sizeof(glm::mat4));
            map[key] = value;
        }
        return map;
    }

    void MaterialSerializer::Serialize(const Ref<Material>& material, const std::filesystem::path& path)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.is_open())
            return;

        // Запись заголовка файла
        uint32_t magic = 0x4D4154; // "MAT"
        uint32_t version = 1;
        stream.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        stream.write(reinterpret_cast<const char*>(&version), sizeof(version));

        // Запись AssetHandle шейдера
        AssetHandle shaderHandle = material->m_ShaderHandle;
        stream.write(reinterpret_cast<const char*>(&shaderHandle), sizeof(shaderHandle));

        // Запись m_NameToID
        WriteNameToIDMap(stream, material->m_NameToID);

        // Запись всех значений
        WriteMapSimple(stream, material->m_IntValues);
        WriteMapSimple(stream, material->m_FloatValues);
        WriteVec2Map(stream, material->m_Vec2Values);
        WriteVec3Map(stream, material->m_Vec3Values);
        WriteVec4Map(stream, material->m_Vec4Values);
        WriteMat3Map(stream, material->m_Mat3Values);
        WriteMat4Map(stream, material->m_Mat4Values);
        WriteMapSimple(stream, material->m_TextureValues);

        // Запись dirty векторов
        WriteVector(stream, material->m_DirtyInts);
        WriteVector(stream, material->m_DirtyFloats);
        WriteVector(stream, material->m_DirtyVec2s);
        WriteVector(stream, material->m_DirtyVec3s);
        WriteVector(stream, material->m_DirtyVec4s);
        WriteVector(stream, material->m_DirtyMat3s);
        WriteVector(stream, material->m_DirtyMat4s);
        WriteVector(stream, material->m_DirtyTextures);

        stream.close();
    }

    DeserializeMaterialResult MaterialSerializer::Deserialize(const std::filesystem::path& path)
    {
        DeserializeMaterialResult res;
        res.isSuccessful = false;
        res.material = nullptr;

        std::ifstream stream(path, std::ios::binary);
        if (!stream.is_open())
            return res;

        // Чтение заголовка
        uint32_t magic, version;
        stream.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        stream.read(reinterpret_cast<char*>(&version), sizeof(version));

        if (magic != 0x4D4154 || version != 1)
            return res;

        // Чтение AssetHandle шейдера
        AssetHandle shaderHandle;
        stream.read(reinterpret_cast<char*>(&shaderHandle), sizeof(shaderHandle));

        // Создание материала
        Ref<Material> material = CreateRef<Material>(shaderHandle);

        // Чтение m_NameToID
        material->m_NameToID = ReadNameToIDMap(stream);

        // Чтение всех значений
        material->m_IntValues = ReadMapSimple<uint32_t, int>(stream);
        material->m_FloatValues = ReadMapSimple<uint32_t, float>(stream);
        material->m_Vec2Values = ReadVec2Map(stream);
        material->m_Vec3Values = ReadVec3Map(stream);
        material->m_Vec4Values = ReadVec4Map(stream);
        material->m_Mat3Values = ReadMat3Map(stream);
        material->m_Mat4Values = ReadMat4Map(stream);
        material->m_TextureValues = ReadMapSimple<uint32_t, AssetHandle>(stream);

        // Чтение dirty векторов
        material->m_DirtyInts = ReadVector<uint32_t>(stream);
        material->m_DirtyFloats = ReadVector<uint32_t>(stream);
        material->m_DirtyVec2s = ReadVector<uint32_t>(stream);
        material->m_DirtyVec3s = ReadVector<uint32_t>(stream);
        material->m_DirtyVec4s = ReadVector<uint32_t>(stream);
        material->m_DirtyMat3s = ReadVector<uint32_t>(stream);
        material->m_DirtyMat4s = ReadVector<uint32_t>(stream);
        material->m_DirtyTextures = ReadVector<uint32_t>(stream);

        stream.close();

        res.isSuccessful = true;
        res.material = material;
        return res;
    }
}