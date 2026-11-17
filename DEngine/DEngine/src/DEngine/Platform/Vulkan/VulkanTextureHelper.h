#pragma once

#include "vulkan/vulkan.h"

namespace DEngine
{
	class VulkanTextureHelper
	{
	public:
		static void TransitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspect);
	};
}
