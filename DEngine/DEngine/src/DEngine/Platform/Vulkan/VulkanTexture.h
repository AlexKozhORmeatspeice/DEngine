#pragma once

#include "DEngine/Renderer/Texture/Texture.h"
#include "DEngine/Platform/Vulkan/VulkanRendererAPI.h"

#include "string"
#include "vector"

namespace DEngine
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(const TextureSpecification& spec, const DataBuffer& data = DataBuffer());
		virtual ~VulkanTexture2D();

		virtual const TextureSpecification& GetSpecification() const { return m_Specification; }

		virtual unsigned int GetWidth() const override { return m_Width; }
		virtual unsigned int GetHeight() const override { return m_Height; }

		virtual void SetData(const DataBuffer& data) override;
		virtual uint32_t GetRendererID() const override;

		virtual void Bind(CommandBuffer& commandBuffer, uint32_t slot = 0) const override;
		virtual void Unbind(CommandBuffer& commandBuffer) const override;
		virtual void* GetImGuiHandle() const override;

		VkImageView GetImageView() const { return m_ImageView; }
		VkSampler GetSampler() const { return m_Sampler; }

	private:
		void CreateImage();
		void Upload(const void* pixels, uint32_t size, uint32_t bytesPerPixel);

		TextureSpecification m_Specification;
		unsigned int m_Width = 0;
		unsigned int m_Height = 0;
		VkFormat m_Format = VK_FORMAT_R8G8B8A8_UNORM;
		VkDevice m_Device = VK_NULL_HANDLE;
		VulkanAllocatedImage m_Image;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;
		VkImageLayout m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
		mutable void* m_ImGuiHandle = nullptr;
	};
}
