#include "dpch.h"
#include "TextureImporter.h"
#include "stb_image.h"

namespace DEngine
{
    Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
    {
        return LoadTexture2D(metadata.FilePath);
    }

    Ref<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path)
    {
        // Проверка существования файла
        if (!std::filesystem::exists(path))
        {
            D_CORE_ERROR("TextureImporter: file not found: {0}", path.string());
            return nullptr;
        }

        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);

        DataBuffer data;
        data.Data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

        if (data.Data == nullptr)
        {
            D_CORE_ERROR("TextureImporter: failed to load texture: {0}", path.string());
            return nullptr;
        }

        // Проверка размеров
        if (width <= 0 || height <= 0)
        {
            D_CORE_ERROR("TextureImporter: invalid dimensions {0}x{1} for {2}", 
                         width, height, path.string());
            data.Release();
            return nullptr;
        }

        TextureSpecification spec;
        spec.Width = width;
        spec.Height = height;

        // Определение формата
        switch (channels)
        {
            case 1:
                spec.Format = ImageFormat::R8;
                break;
            case 3:
                spec.Format = ImageFormat::RGB8;
                break;
            case 4:
                spec.Format = ImageFormat::RGBA8;
                break;
            default:
                D_CORE_ERROR("TextureImporter: unsupported channels {0} in {1}", 
                             channels, path.string());
                data.Release();
                return nullptr;
        }

        data.Size = width * height * channels;

        // Отладочный вывод
        D_CORE_INFO("TextureImporter: loaded {0} ({1}x{2}, channels={3}, format={4}, size={5} bytes)",
                    path.filename().string(), width, height, channels, 
                    (int)spec.Format, data.Size);

        Ref<Texture2D> tex = Texture2D::Create(spec, data);
        data.Release();

        return tex;
    }
}