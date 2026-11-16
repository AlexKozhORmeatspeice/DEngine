#pragma once

#include "string"
#include "cstdint"

namespace DEngine
{
	enum class DPrimitiveTopology
	{
		TriangleList
	};

	enum class DPolygonMode
	{
		Fill
	};

	enum class DCullMode
	{
		None
	};

	enum class DFrontFace
	{
		Clockwise
	};

	enum class DSampleCount
	{
		Count1 = 1
	};

	enum class DBlendFactor
	{
		Zero,
		One
	};

	enum class DBlendOp
	{
		Add
	};

	enum class DLogicOp
	{
		Copy
	};

	enum class DCompareOp
	{
		Less
	};

	struct DColorMask
	{
		static constexpr uint32_t R = 1 << 0;
		static constexpr uint32_t G = 1 << 1;
		static constexpr uint32_t B = 1 << 2;
		static constexpr uint32_t A = 1 << 3;
		static constexpr uint32_t RGBA = R | G | B | A;
	};

	struct DViewport
	{
		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
		float minDepth = 0.0f;
		float maxDepth = 1.0f;
	};

	struct DOffset
	{
		int32_t x = 0;
		int32_t y = 0;
	};

	struct DExtent
	{
		uint32_t width = 0;
		uint32_t height = 0;
	};

	struct DScissor
	{
		DOffset offset;
		DExtent extent;
	};

	struct DViewportState
	{
		uint32_t viewportCount = 1;
		uint32_t scissorCount = 1;
	};

	struct DInputAssembly
	{
		DPrimitiveTopology topology = DPrimitiveTopology::TriangleList;
		bool primitiveRestart = false;
	};

	struct DRasterization
	{
		bool depthClamp = false;
		bool rasterizerDiscard = false;
		DPolygonMode polygonMode = DPolygonMode::Fill;
		float lineWidth = 1.0f;
		DCullMode cullMode = DCullMode::None;
		DFrontFace frontFace = DFrontFace::Clockwise;
		bool depthBias = false;
		float depthBiasConstant = 0.0f;
		float depthBiasClamp = 0.0f;
		float depthBiasSlope = 0.0f;
	};

	struct DMultisample
	{
		bool sampleShading = false;
		DSampleCount rasterizationSamples = DSampleCount::Count1;
		float minSampleShading = 1.0f;
		const uint32_t* sampleMask = nullptr;
		bool alphaToCoverage = false;
		bool alphaToOne = false;
	};

	struct DColorBlendAttachment
	{
		uint32_t colorWriteMask = DColorMask::RGBA;
		bool blend = false;
		DBlendFactor srcColorBlendFactor = DBlendFactor::One;
		DBlendFactor dstColorBlendFactor = DBlendFactor::Zero;
		DBlendOp colorBlendOp = DBlendOp::Add;
		DBlendFactor srcAlphaBlendFactor = DBlendFactor::One;
		DBlendFactor dstAlphaBlendFactor = DBlendFactor::Zero;
		DBlendOp alphaBlendOp = DBlendOp::Add;
	};

	struct DColorBlend
	{
		bool logicOpEnable = false;
		DLogicOp logicOp = DLogicOp::Copy;
		uint32_t attachmentCount = 1;
		float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	struct DStencilOpState
	{
	};

	struct DDepthStencil
	{
		bool depthTest = true;
		bool depthWrite = true;
		DCompareOp depthCompareOp = DCompareOp::Less;
		bool depthBoundsTest = false;
		float minDepthBounds = 0.0f;
		float maxDepthBounds = 1.0f;
		bool stencilTest = false;
		DStencilOpState front;
		DStencilOpState back;
	};

	struct DPipelineLayout
	{
		void* native = nullptr;
	};

	struct DRenderPass
	{
		void* native = nullptr;
	};

	struct PipelineState
	{
		DViewport viewport;
		DScissor scissor;
		DInputAssembly inputAssembly;
		DRasterization rasterization;
		DMultisample multisample;
		DColorBlendAttachment colorBlendAttachment;
		DColorBlend colorBlend;
		DDepthStencil depthStencil;
		DPipelineLayout pipelineLayout;
		DRenderPass renderPass;
		uint32_t subpass = 0;
	};
}
