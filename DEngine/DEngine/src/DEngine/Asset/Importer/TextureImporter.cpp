#include "dpch.h"
#include "TextureImporter.h"
#include "stb_image.h"

#include "DEngine/Jobs/JobSystem.h"

#include <mutex>
#include "DEngine/Profiler/Profiler.h"

namespace DEngine
{
    namespace
    {
        Ref<Texture2D> UploadTexture(const TextureSpecification& spec, const DataBuffer& data)
        {
            const bool onMainThread = !JobSystem::IsInitialized() || JobSystem::ThreadIndex() == 0;
            if (onMainThread)
                return Texture2D::Create(spec, data);

            Ref<Texture2D> texture;
            PinnedJob upload([&]()
            {
                texture = Texture2D::Create(spec, data);
            });
            JobSystem::Schedule(upload);
            JobSystem::Wait(upload);
            return texture;
        }
    }

    Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
    {
        return LoadTexture2D(metadata.FilePath);
    }

    Ref<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path)
    {
        DecodedTexture decoded = DecodeTexture2D(path);
        if (!decoded.Succeeded)
            return nullptr;

        Ref<Texture2D> tex = UploadTexture(decoded.Spec, decoded.Pixels);
        decoded.Pixels.Release();
        return tex;
    }

    DecodedTexture TextureImporter::DecodeTexture2D(const std::filesystem::path& path)
    {
        DecodedTexture decoded;

        if (!std::filesystem::exists(path))
        {
            D_CORE_ERROR("TextureImporter: file not found: {0}", path.string());
            return decoded;
        }

        int width, height, channels;
        static std::once_flag flipOnce;
        std::call_once(flipOnce, []() { stbi_set_flip_vertically_on_load(1); });

        decoded.Pixels.Data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
        if (decoded.Pixels.Data == nullptr)
        {
            D_CORE_ERROR("TextureImporter: failed to load texture: {0}", path.string());
            return decoded;
        }

        if (width <= 0 || height <= 0)
        {
            D_CORE_ERROR("TextureImporter: invalid dimensions {0}x{1} for {2}",
                         width, height, path.string());
            decoded.Pixels.Release();
            return decoded;
        }

        decoded.Spec.Width = width;
        decoded.Spec.Height = height;

        switch (channels)
        {
            case 1:
                decoded.Spec.Format = ImageFormat::R8;
                break;
            case 3:
                decoded.Spec.Format = ImageFormat::RGB8;
                break;
            case 4:
                decoded.Spec.Format = ImageFormat::RGBA8;
                break;
            default:
                D_CORE_ERROR("TextureImporter: unsupported channels {0} in {1}",
                             channels, path.string());
                decoded.Pixels.Release();
                return decoded;
        }

        decoded.Pixels.Size = width * height * channels;
        decoded.Succeeded = true;

        D_CORE_INFO("TextureImporter: loaded {0} ({1}x{2}, channels={3}, format={4}, size={5} bytes)",
                    path.filename().string(), width, height, channels,
                    (int)decoded.Spec.Format, decoded.Pixels.Size);

        return decoded;
    }
}
