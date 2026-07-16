#include "dpch.h"
#include "MeshSerializer.h"
#include "DEngine/Asset/AssetManager.h"

#include <fstream>

namespace DEngine
{
    void MeshSerializer::Serialize(const AssetHandle& handle, const Ref<Mesh>& mesh, const MeshData& data, const std::filesystem::path& path)
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

        // 1. Write handle
        uint64_t handleValue = static_cast<uint64_t>(handle);
        file.write(reinterpret_cast<const char*>(&handleValue), sizeof(handleValue));

        // 2. Write asset type
        uint16_t assetType = static_cast<uint16_t>(AssetType::Mesh);
        file.write(reinterpret_cast<const char*>(&assetType), sizeof(assetType));

        // 3. Write layout
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

        // 4. Write stride
        uint32_t stride = mesh->GetLayout().GetStride();
        file.write(reinterpret_cast<const char*>(&stride), sizeof(stride));

        // 5. Write vertex data
        file.write(reinterpret_cast<const char*>(&data.vertSize), sizeof(data.vertSize));
        if (data.vertSize > 0 && data.verts)
        {
            file.write(reinterpret_cast<const char*>(data.verts), data.vertSize);
        }

        // 6. Write index data
        file.write(reinterpret_cast<const char*>(&data.indsSize), sizeof(data.indsSize));
        if (data.indsSize > 0 && data.inds)
        {
            file.write(reinterpret_cast<const char*>(data.inds), data.indsSize * sizeof(uint32_t));
        }

        file.close();

        D_CORE_INFO("MeshSerializer::SerializeBinary: mesh serialized to {0} (vertices: {1}, indices: {2})",
            path.string(), data.vertSize / stride, data.indsSize);
    }

    DeserializeMeshResult MeshSerializer::Deserialize(const std::filesystem::path& path)
    {
        DeserializeMeshResult result;
        result.isSuccessful = false;
        result.mesh = nullptr;
        result.handle = AssetHandle::Invalid();

        if (!std::filesystem::exists(path))
        {
            D_CORE_ERROR("MeshSerializer::DeserializeBinary: file not found: {0}", path.string());
            return result;
        }

        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            D_CORE_ERROR("MeshSerializer::DeserializeBinary: failed to open file: {0}", path.string());
            return result;
        }

        try
        {
            // 1. Read handle
            uint64_t handleValue;
            file.read(reinterpret_cast<char*>(&handleValue), sizeof(handleValue));
            result.handle = static_cast<AssetHandle>(handleValue);

            // 2. Read asset type
            uint16_t assetType;
            file.read(reinterpret_cast<char*>(&assetType), sizeof(assetType));

            if (static_cast<AssetType>(assetType) != AssetType::Mesh)
            {
                D_CORE_ERROR("MeshSerializer::DeserializeBinary: invalid asset type in file: {0}", path.string());
                return result;
            }

            // 3. Read layout
            BufferLayout layout;
            uint32_t elementCount;
            file.read(reinterpret_cast<char*>(&elementCount), sizeof(elementCount));

            for (uint32_t i = 0; i < elementCount; i++)
            {
                BufferElement element;

                // Name
                uint32_t nameLen;
                file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
                element.Name.resize(nameLen);
                file.read(element.Name.data(), nameLen);

                // Type
                uint32_t type;
                file.read(reinterpret_cast<char*>(&type), sizeof(type));
                element.Type = static_cast<ShaderDataType>(type);

                // Size
                file.read(reinterpret_cast<char*>(&element.Size), sizeof(element.Size));

                // Offset
                file.read(reinterpret_cast<char*>(&element.Offset), sizeof(element.Offset));

                layout.AddElement(element);
            }
            layout.CalculateOffsetAndStride();

            // 4. Read stride
            uint32_t stride;
            file.read(reinterpret_cast<char*>(&stride), sizeof(stride));

            // 5. Read vertex data
            uint32_t vertSize;
            file.read(reinterpret_cast<char*>(&vertSize), sizeof(vertSize));

            float* verts = nullptr;
            if (vertSize > 0)
            {
                verts = new float[vertSize / sizeof(float)];
                file.read(reinterpret_cast<char*>(verts), vertSize);
            }

            // 6. Read index data
            uint32_t indsSize;
            file.read(reinterpret_cast<char*>(&indsSize), sizeof(indsSize));

            uint32_t* inds = nullptr;
            if (indsSize > 0)
            {
                inds = new uint32_t[indsSize];
                file.read(reinterpret_cast<char*>(inds), indsSize * sizeof(uint32_t));
            }

            // Create mesh data
            MeshData meshData;
            meshData.verts = verts;
            meshData.vertSize = vertSize;
            meshData.inds = inds;
            meshData.indsSize = indsSize;

            // Create mesh
            result.mesh = CreateRef<Mesh>(layout, meshData);
            result.isSuccessful = true;

            D_CORE_INFO("MeshSerializer::DeserializeBinary: mesh deserialized from {0} (vertices: {1}, indices: {2})",
                path.string(), vertSize / stride, indsSize);

            return result;
        }
        catch (const std::exception& e)
        {
            D_CORE_ERROR("MeshSerializer::DeserializeBinary: exception: {0}", e.what());
            return result;
        }
    }
}