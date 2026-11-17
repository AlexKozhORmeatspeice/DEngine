#include "dpch.h"
#include "DEngine/Renderer/Pipeline/RendererPipeline.h"

namespace DEngine
{
	PipelineState RendererPipeline::CreateDefaultPipelineState(uint32_t width, uint32_t height)
	{
		PipelineState state;

		state.inputAssembly.topology = DPrimitiveTopology::TriangleList;
		state.inputAssembly.primitiveRestart = false;

		state.viewport.x = 0.0f;
		state.viewport.y = 0.0f;
		state.viewport.width = static_cast<float>(width);
		state.viewport.height = static_cast<float>(height);
		state.viewport.minDepth = 0.0f;
		state.viewport.maxDepth = 1.0f;

		state.scissor.offset = { 0, 0 };
		state.scissor.extent = { width, height };

		state.rasterization.depthClamp = false;
		state.rasterization.rasterizerDiscard = false;
		state.rasterization.polygonMode = DPolygonMode::Fill;
		state.rasterization.lineWidth = 1.0f;
		state.rasterization.cullMode = DCullMode::None;
		state.rasterization.frontFace = DFrontFace::Clockwise;
		state.rasterization.depthBias = false;
		state.rasterization.depthBiasConstant = 0.0f;
		state.rasterization.depthBiasClamp = 0.0f;
		state.rasterization.depthBiasSlope = 0.0f;

		state.multisample.sampleShading = false;
		state.multisample.rasterizationSamples = DSampleCount::Count1;
		state.multisample.minSampleShading = 1.0f;
		state.multisample.sampleMask = nullptr;
		state.multisample.alphaToCoverage = false;
		state.multisample.alphaToOne = false;

		state.colorBlendAttachment.colorWriteMask = DColorMask::RGBA;
		state.colorBlendAttachment.blend = false;
		state.colorBlendAttachment.srcColorBlendFactor = DBlendFactor::One;
		state.colorBlendAttachment.dstColorBlendFactor = DBlendFactor::Zero;
		state.colorBlendAttachment.colorBlendOp = DBlendOp::Add;
		state.colorBlendAttachment.srcAlphaBlendFactor = DBlendFactor::One;
		state.colorBlendAttachment.dstAlphaBlendFactor = DBlendFactor::Zero;
		state.colorBlendAttachment.alphaBlendOp = DBlendOp::Add;

		state.colorBlend.logicOpEnable = false;
		state.colorBlend.logicOp = DLogicOp::Copy;
		state.colorBlend.attachmentCount = 1;
		state.colorBlend.blendConstants[0] = 0.0f;
		state.colorBlend.blendConstants[1] = 0.0f;
		state.colorBlend.blendConstants[2] = 0.0f;
		state.colorBlend.blendConstants[3] = 0.0f;

		state.depthStencil.depthTest = true;
		state.depthStencil.depthWrite = true;
		state.depthStencil.depthCompareOp = DCompareOp::Less;
		state.depthStencil.depthBoundsTest = false;
		state.depthStencil.minDepthBounds = 0.0f;
		state.depthStencil.maxDepthBounds = 1.0f;
		state.depthStencil.stencilTest = false;
		state.depthStencil.front = {};
		state.depthStencil.back = {};

		state.vertexLayout =
		{
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_Texcoord" },
			{ ShaderDataType::Float3, "a_Tangent" },
		};

		state.subpass = 0;

		return state;
	}
}
