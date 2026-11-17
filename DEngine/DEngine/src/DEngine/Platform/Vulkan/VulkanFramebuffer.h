#pragma once

#include "DEngine/Renderer/Buffers/Framebuffer.h"
#include "DEngine/Platform/Vulkan/VulkanRendererAPI.h"
#include "vulkan/vulkan.h"

namespace DEngine
{
	class RenderDevice;

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		VulkanFramebuffer(RenderDevice& device, const FramebufferState& state, VkRenderPass renderPass, VkImageView colorView, VkImageView depthView);
		virtual ~VulkanFramebuffer();

		virtual void Bind(CommandBuffer& commandBuffer) override;
		virtual void Unbind(CommandBuffer& commandBuffer) override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void* GetImGuiHandle() const override;

		void Invalidate();
		virtual const FramebufferSpecification& GetSpecification() const { return m_Specification; }

		virtual uint32_t GetColorAttachmentRendererID() const { return m_ColorAttachment; }
		VkFramebuffer GetNative() const { return m_Framebuffer; }
	private:
		void CreateOffscreen();
		void ReleaseOffscreen();

		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0;
		uint32_t m_DepthAttachment = 0;
		FramebufferSpecification m_Specification;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
		VulkanAllocatedImage m_ColorImage;
		VulkanAllocatedImage m_DepthImage;
		VkImageView m_ColorView = VK_NULL_HANDLE;
		VkImageView m_DepthView = VK_NULL_HANDLE;
		bool m_Offscreen = false;
		mutable void* m_ImGuiHandle = nullptr;
	};
}
