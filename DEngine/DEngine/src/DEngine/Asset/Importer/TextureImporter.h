#pragma once
#include "DEngine/Core.h"

#include "DEngine/Asset/Asset.h"
#include "DEngine/Asset/AssetMetadata.h"

#include "DEngine/Renderer/Texture/Texture.h"

namespace DEngine
{
	struct DecodedTexture
	{
		TextureSpecification Spec;
		DataBuffer Pixels;
		bool Succeeded = false;
	};

	class TextureImporter
	{
	public:
		static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& path);
		static DecodedTexture DecodeTexture2D(const std::filesystem::path& path);
	};
}
