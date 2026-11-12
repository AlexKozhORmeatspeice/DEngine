#include "dpch.h"
#include "MeshSerializer.h"
#include "DEngine/Asset/AssetManager.h"

#include <fstream>

namespace DEngine
{
    void MeshSerializer::Serialize(const Ref<Mesh>& mesh, const MeshData& data, const std::filesystem::path& path)
    {
        if (!mesh)
        {
            D_CORE_ERROR("MeshSerializer::SerializeBinary: mesh is null!");
            return;
        }

        std::ofstream file(path, std::ios::binary);
        if (!file)
        {
            D_CORE_ERROR("MeshSerializer::SerializeBinary: failed to open file: {0}", path.string());
            return;
        }

        // 1. Write asset type
        uint16_t assetType = static_cast<uint16_t>(AssetType::Mesh);
        file.write(reinterpret_cast<const char*>(&assetType), sizeof(assetType));

        // 2. Write layout
        const auto& layoutElements = mesh->GetLayout().GetElements();
        uint32_t elementCount = static_cast<uint32_t>(layoutElements.size());
        file.write(reinterpret_cast<const char*>(&elementCount), sizeof(elementCount));

        for (const auto& element : layoutElements)
        {
            // Name
            uint32_t nameLen = static_cast<uint32_t>(element.Name.size());
            file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            file.write(element.Name.c_str(), nameLen);

            // Type
            uint32_t type = static_cast<uint32_t>(element.Type);
            file.write(reinterpret_cast<const char*>(&type), sizeof(type));

            // Size
            file.write(reinterpret_cast<const char*>(&element.Size), sizeof(element.Size));

            // Offset
            file.write(reinterpret_cast<const char*>(&element.Offset), sizeof(element.Offset));
        }

        // 3. Write stride
        uint32_t stride = mesh->GetLayout().GetStride();
        file.write(reinterpret_cast<const char*>(&stride), sizeof(stride));

        // 4. Write vertex data
        file.write(reinterpret_cast<const char*>(&data.vertSize), sizeof(data.vertSize));
        if (data.vertSize > 0 && data.verts)
        {
            file.write(reinterpret_cast<const char*>(data.verts), data.vertSize);
        }

        // 5. Write index data
        file.write(reinterpret_cast<const char*>(&data.indsSize), sizeof(data.indsSize));
        if (data.indsSize > 0 && data.inds)
        {
            file.write(reinterpret_cast<const char*>(data.inds), data.indsSize * sizeof(uint32_t));
        }

        file.close();

        D_CORE_INFO("MeshSerializer::SerializeBinary: mesh serialized to {0} (vertices: {1}, indices: {2})",
            path.string(), data.vertSize / stride, data.indsSize);
    }

    MeshFile MeshSerializer::Read(const std::filesystem::path& path)
    {
        MeshFile file;

        if (!std::filesystem::exists(path))
        {
            D_CORE_ERROR("MeshSerializer::Read: file not found: {0}", path.string());
            return file;
        }

        std::ifstream stream(path, std::ios::binary);
        if (!stream)
        {
            D_CORE_ERROR("MeshSerializer::Read: failed to open file: {0}", path.string());
            return file;
        }

        float* verts = nullptr;
        uint32_t* inds = nullptr;

        try
        {
            uint16_t assetType;
            stream.read(reinterpret_cast<char*>(&assetType), sizeof(assetType));

            if (static_cast<AssetType>(assetType) != AssetType::Mesh)
            {
                D_CORE_ERROR("MeshSerializer::Read: invalid asset type in file: {0}", path.string());
                return file;
            }

            BufferLayout layout;
            uint32_t elementCount;
            stream.read(reinterpret_cast<char*>(&elementCount), sizeof(elementCount));

            for (uint32_t i = 0; i < elementCount; i++)
            {
                BufferElement element;

                uint32_t nameLen;
                stream.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
                element.Name.resize(nameLen);
                stream.read(element.Name.data(), nameLen);

                uint32_t type;
                stream.read(reinterpret_cast<char*>(&type), sizeof(type));
                element.Type = static_cast<ShaderDataType>(type);

                stream.read(reinterpret_cast<char*>(&element.Size), sizeof(element.Size));
                stream.read(reinterpret_cast<char*>(&element.Offset), sizeof(element.Offset));

                layout.AddElement(element);
            }
            layout.CalculateOffsetAndStride();

            uint32_t stride;
            stream.read(reinterpret_cast<char*>(&stride), sizeof(stride));

            uint32_t vertSize;
            stream.read(reinterpret_cast<char*>(&vertSize), sizeof(vertSize));

            if (vertSize > 0)
            {
                verts = new float[vertSize / sizeof(float)];
                stream.read(reinterpret_cast<char*>(verts), vertSize);
            }

            uint32_t indsSize;
            stream.read(reinterpret_cast<char*>(&indsSize), sizeof(indsSize));

            if (indsSize > 0)
            {
                inds = new uint32_t[indsSize];
                stream.read(reinterpret_cast<char*>(inds), indsSize * sizeof(uint32_t));
            }

            file.Layout = layout;
            file.Data.verts = verts;
            file.Data.vertSize = vertSize;
            file.Data.inds = inds;
            file.Data.indsSize = indsSize;
            file.Succeeded = true;

            D_CORE_INFO("MeshSerializer::Read: mesh read from {0} (vertices: {1}, indices: {2})",
                path.string(), stride > 0 ? vertSize / stride : 0, indsSize);

            return file;
        }
        catch (const std::exception& e)
        {
            delete[] verts;
            delete[] inds;
            D_CORE_ERROR("MeshSerializer::Read: exception: {0}", e.what());
            return MeshFile();
        }
    }

    DeserializeMeshResult MeshSerializer::Deserialize(const std::filesystem::path& path)
    {
        DeserializeMeshResult result;
        result.isSuccessful = false;
        result.mesh = nullptr;

        MeshFile file = Read(path);
        if (!file.Succeeded)
            return result;

        result.mesh = CreateRef<Mesh>(file.Layout, file.Data);
        file.Data.Release();
        result.isSuccessful = result.mesh != nullptr;
        return result;
    }
}