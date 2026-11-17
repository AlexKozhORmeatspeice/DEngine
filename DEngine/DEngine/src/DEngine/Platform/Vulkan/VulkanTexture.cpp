#include "dpch.h"
#include "VulkanTexture.h"

#include "DEngine/Application.h"
#include "DEngine/Platform/Vulkan/VulkanImGuiHelper.h"
#include "DEngine/Platform/Vulkan/VulkanRenderDevice.h"
#include "DEngine/Platform/Vulkan/VulkanTextureHelper.h"

#include "vector"

namespace DEngine
{
	static VkFormat ImageFormatToVulkan(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::R8: return VK_FORMAT_R8_UNORM;
			case ImageFormat::RGB8: return VK_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::RGBA8: return VK_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
			default: break;
		}
		return VK_FORMAT_R8G8B8A8_UNORM;
	}

	VulkanTexture2D::VulkanTexture2D(const TextureSpecification& spec, const DataBuffer& data)
		: m_Specification(spec)
		, m_Width(spec.Width)
		, m_Height(spec.Height)
		, m_Format(ImageFormatToVulkan(spec.Format))
		, m_Device(*static_cast<VulkanRenderDevice&>(Application::Get().GetDevice()).Device())
	{
		CreateImage();
		if (data)
			SetData(data);
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		if (m_Device == VK_NULL_HANDLE)
			return;

		if (m_Sampler != VK_NULL_HANDLE)
			vkDestroySampler(m_Device, m_Sampler, nullptr);
		if (m_ImageView != VK_NULL_HANDLE)
			vkDestroyImageView(m_Device, m_ImageView, nullptr);
		if (VulkanRendererAPI::TryGet())
			VulkanRendererAPI::Get().FreeImage(m_Image);
	}

	void VulkanTexture2D::CreateImage()
	{
		if (m_Width == 0 || m_Height == 0)
			return;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = m_Width;
		imageInfo.extent.height = m_Height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = m_Format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		m_Image = VulkanRendererAPI::Get().AllocateImage(imageInfo);
		D_CORE_ASSERT(m_Image.Image != VK_NULL_HANDLE, "Failed to allocate texture image");

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image.Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = m_Format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.layerCount = 1;
		D_CORE_ASSERT(vkCreateImageView(m_Device, &viewInfo, nullptr, &m_ImageView) == VK_SUCCESS, "Failed to create texture image view");

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		D_CORE_ASSERT(vkCreateSampler(m_Device, &samplerInfo, nullptr, &m_Sampler) == VK_SUCCESS, "Failed to create texture sampler");
	}

	void VulkanTexture2D::Upload(const void* pixels, uint32_t size, uint32_t bytesPerPixel)
	{
		if (!pixels || m_Image.Image == VK_NULL_HANDLE || size == 0)
			return;

		VulkanAllocatedBuffer staging = VulkanRendererAPI::Get().AllocateBuffer(
			size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

		void* mapped = nullptr;
		VmaAllocator allocator = VulkanRendererAPI::Get().GetAllocator();
		D_CORE_ASSERT(vmaMapMemory(allocator, staging.Allocation, &mapped) == VK_SUCCESS, "Failed to map texture staging buffer");
		memcpy(mapped, pixels, size);
		vmaFlushAllocation(allocator, staging.Allocation, 0, size);
		vmaUnmapMemory(allocator, staging.Allocation);

		VulkanRenderDevice& device = static_cast<VulkanRenderDevice&>(Application::Get().GetDevice());
		VkCommandBuffer commandBuffer = device.BeginSingleTimeCommands();
		VulkanTextureHelper::TransitionImage(commandBuffer, m_Image.Image, m_Layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

		VkBufferImageCopy region{};
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.layerCount = 1;
		region.imageExtent = { m_Width, m_Height, 1 };
		vkCmdCopyBufferToImage(commandBuffer, staging.Buffer, m_Image.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		VulkanTextureHelper::TransitionImage(commandBuffer, m_Image.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
		device.EndSingleTimeCommands(commandBuffer);
		m_Layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		VulkanRendererAPI::Get().FreeBuffer(staging);
	}

	void VulkanTexture2D::SetData(const DataBuffer& data)
	{
		if (!data || m_Width == 0 || m_Height == 0)
			return;

		if (m_Specification.Format == ImageFormat::RGB8)
		{
			std::vector<uint8_t> expanded(static_cast<size_t>(m_Width) * m_Height * 4, 255);
			const uint8_t* source = static_cast<const uint8_t*>(data.Data);
			for (uint32_t i = 0; i < m_Width * m_Height; i++)
			{
				expanded[i * 4 + 0] = source[i * 3 + 0];
				expanded[i * 4 + 1] = source[i * 3 + 1];
				expanded[i * 4 + 2] = source[i * 3 + 2];
			}
			Upload(expanded.data(), static_cast<uint32_t>(expanded.size()), 4);
			return;
		}

		uint32_t bytesPerPixel = m_Format == VK_FORMAT_R8_UNORM ? 1 : (m_Format == VK_FORMAT_R32G32B32A32_SFLOAT ? 16 : 4);
		Upload(data.Data, data.Size, bytesPerPixel);
	}

	uint32_t VulkanTexture2D::GetRendererID() const
	{
		return 0;
	}

	void VulkanTexture2D::Bind(CommandBuffer& commandBuffer, uint32_t slot) const
	{
	}

	void VulkanTexture2D::Unbind(CommandBuffer& commandBuffer) const
	{
	}

	void* VulkanTexture2D::GetImGuiHandle() const
	{
		if (!m_ImGuiHandle && VulkanImGuiHelper::IsReady())
			m_ImGuiHandle = VulkanImGuiHelper::RegisterTexture(m_ImageView, m_Sampler);
		return m_ImGuiHandle;
	}
}
