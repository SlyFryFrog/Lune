module;
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <iostream>
module lune.metal;

import lune;
import lune.gfx;

namespace lune::metal
{
	MTL::Library* createLibrary(const std::string& path, MTL::Device* device, NS::Error** error)
	{
		MTL::Library* library{};

		if (path.ends_with(".metal")) // Runtime-compiled shader
		{
			const auto shaderSource = File::read(path);
			if (!shaderSource.has_value())
			{
				if (error)
					*error = NS::Error::alloc()->init();
				std::cerr << "Shader file not found: " << path << "\n";
				return nullptr;
			}

			const auto nsSource =
					NS::String::string(shaderSource.value().c_str(), NS::UTF8StringEncoding);
			library = device->newLibrary(nsSource, nullptr, error);
		}
		else if (path.ends_with(".metallib")) // Precompiled shader
		{
			const auto nsPath = NS::String::string(path.c_str(), NS::UTF8StringEncoding);
			library = device->newLibrary(nsPath, error);
		}

		return library;
	}

	void MetalShaderImpl::create()
	{
		NS::Error* error{};
		m_library = NS::TransferPtr(createLibrary(m_desc.path, m_device, &error));

		if (error)
		{
			if (const auto desc = error->localizedDescription())
				std::cerr << "Failed to create library: " << desc->cString(NS::UTF8StringEncoding)
						  << "\n";
			else
				std::cerr << "Failed to create library: unknown error\n";
		}
		else
		{
			// If library creation succeeded, fetch functions
			if (m_library)
			{
				m_vertex = NS::TransferPtr(m_library->newFunction(
						NS::String::string(m_desc.vsMain.c_str(), NS::UTF8StringEncoding)));
				m_fragment = NS::TransferPtr(m_library->newFunction(
						NS::String::string(m_desc.fsMain.c_str(), NS::UTF8StringEncoding)));
			}
		}
	}

	void MetalMaterialImpl::setUniform(const std::string& name, const gfx::Texture& texture)
	{
		m_textures[name] = toMetalImpl(texture)->texture();
	}

	void MetalMaterialImpl::setUniform(const std::string& name, const gfx::Buffer& buffer)
	{
		m_uniformBuffers[name] = toMetalImpl(buffer)->buffer();
	}

	void MetalMaterialImpl::setUniform(const std::string& name, const void* data, size_t size)
	{
		auto buffer =
				MetalContext::instance().device()->newBuffer(size, MTL::ResourceStorageModeShared);
		if (!buffer)
		{
			std::cerr << "Failed to create uniform buffer for '" << name << "'\n";
			return;
		}

		// Copy data to the newly created buffer and set as uniform
		std::memcpy(buffer->contents(), data, size);
		m_uniformBuffers[name] = buffer;
	}

	void MetalMaterialImpl::bind(MTL::RenderCommandEncoder* encoder) const
	{
		if (!encoder)
			return;

		const MetalPipelineImpl* metalPipeline{toMetalImpl(*m_pipeline)};

		// Iterate through all vertex and fragment args and set buffers
		for (auto& arg : metalPipeline->vertexArguments())
		{
			if (arg.type == MTL::ArgumentTypeBuffer)
			{
				auto it = m_uniformBuffers.find(arg.name);
				if (it != m_uniformBuffers.end())
				{
					encoder->setVertexBuffer(it->second, 0, arg.index);
				}
			}
			else if (arg.type == MTL::ArgumentTypeTexture)
			{
				auto it = m_textures.find(arg.name);
				if (it != m_textures.end())
				{
					encoder->setVertexTexture(it->second, arg.index);
				}
			}
		}

		for (auto& arg : metalPipeline->fragmentArguments())
		{
			if (arg.type == MTL::ArgumentTypeBuffer)
			{
				auto it = m_uniformBuffers.find(arg.name);
				if (it != m_uniformBuffers.end())
				{
					encoder->setFragmentBuffer(it->second, 0, arg.index);
				}
			}
			else if (arg.type == MTL::ArgumentTypeTexture)
			{
				auto it = m_textures.find(arg.name);
				if (it != m_textures.end())
				{
					encoder->setFragmentTexture(it->second, arg.index);
				}
			}
		}
	}

	void MetalPipelineImpl::createPipeline()
	{
		NS::Error* error{};
		MTL::RenderPipelineReflection* reflection{};
		const MetalShaderImpl* metalShader{toMetalImpl(*m_shader)};

		const auto descriptor{NS::TransferPtr(MTL::RenderPipelineDescriptor::alloc()->init())};

		descriptor->setVertexFunction(metalShader->vertex());
		descriptor->setFragmentFunction(metalShader->fragment());
		descriptor->colorAttachments()->object(0)->setPixelFormat(toMetal(m_desc.colorFormat));
		descriptor->setDepthAttachmentPixelFormat(toMetal(m_desc.depthFormat));

		// Configure simple depth/blend state from desc
		auto* colorAttachment{descriptor->colorAttachments()->object(0)};
		if (m_desc.enableBlending)
		{
			colorAttachment->setBlendingEnabled(true);
			colorAttachment->setRgbBlendOperation(MTL::BlendOperationAdd);
			colorAttachment->setAlphaBlendOperation(MTL::BlendOperationAdd);
			colorAttachment->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
			colorAttachment->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
			colorAttachment->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
			colorAttachment->setDestinationAlphaBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
		}

		const auto depthDesc{NS::TransferPtr(MTL::DepthStencilDescriptor::alloc()->init())};
		depthDesc->setDepthWriteEnabled(true);
		depthDesc->setDepthCompareFunction(MTL::CompareFunctionLess);
		m_depthStencilState =
				NS::TransferPtr(metalShader->device()->newDepthStencilState(depthDesc.get()));


		// Create pipeline state with reflection info (request argument info)
		m_state = NS::TransferPtr(metalShader->device()->newRenderPipelineState(
				descriptor.get(), MTL::PipelineOptionArgumentInfo, &reflection, &error));

		if (!m_state)
		{
			if (error && error->localizedDescription())
				std::cerr << "Failed to create pipeline state: "
						  << error->localizedDescription()->cString(NS::UTF8StringEncoding) << "\n";
			else
				std::cerr << "Failed to create pipeline state: unknown error\n";

			return;
		}

		m_vertexArguments = parse(reflection->vertexArguments());
		m_fragmentArguments = parse(reflection->fragmentArguments());
	}

	std::vector<MetalPipelineImpl::ArgumentInfo>
	MetalPipelineImpl::parse(const NS::Array* arguments)
	{
		std::vector<ArgumentInfo> out;
		out.reserve(arguments->count());

		for (int i = 0; i < arguments->count(); ++i)
		{
			// Extract the argument name
			const auto arg = reinterpret_cast<MTL::Argument*>(arguments->object(i));
			const NS::String* nameObj = arg->name();
			const std::string name = nameObj ? nameObj->utf8String() : "<null>";

			// Add argument to array with all relevant information
			out.push_back({
					.name = name,
					.index = static_cast<uint32_t>(arg->index()),
					.arrayLength = static_cast<uint32_t>(arg->arrayLength()),
					.type = arg->type(),
			});
		}

		return out;
	}

	void MetalRenderPassImpl::bind(const gfx::IMaterialImpl& material)
	{
		auto metalMaterial = static_cast<const MetalMaterialImpl&>(material);

		const auto pipeline = toMetalImpl(metalMaterial.pipeline());
		m_encoder->setRenderPipelineState(pipeline->state());
		m_encoder->setDepthStencilState(pipeline->depthStencilState());
		m_encoder->setCullMode(toMetal(pipeline->cullMode()));
		m_encoder->setFrontFacingWinding(toMetal(pipeline->winding()));
		metalMaterial.bind(m_encoder.get());
	}

	void MetalRenderPassImpl::begin()
	{
		const auto* drawable{
				static_cast<CA::MetalDrawable*>(toMetalImpl(m_surface)->nextDrawable())};
		if (!drawable)
		{
			std::cerr << "RenderPass::begin(): drawable is null\n";
			return;
		}

		m_commandBuffer = NS::TransferPtr(MetalContext::instance().commandQueue()->commandBuffer());

		const auto renderPassDescriptor{
				NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init())};
		MTL::RenderPassColorAttachmentDescriptor* colorAttachmentDescriptor{
				renderPassDescriptor->colorAttachments()->object(0)};
		colorAttachmentDescriptor->setTexture(drawable->texture());
		colorAttachmentDescriptor->setLoadAction(MTL::LoadActionClear);
		colorAttachmentDescriptor->setClearColor(MTL::ClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		colorAttachmentDescriptor->setStoreAction(MTL::StoreActionStore);

		m_encoder =
				NS::TransferPtr(m_commandBuffer->renderCommandEncoder(renderPassDescriptor.get()));
	}

	void MetalRenderPassImpl::end()
	{
		const auto drawable{
				static_cast<CA::MetalDrawable*>(toMetalImpl(m_surface)->currentDrawable())};

		if (!drawable)
			return;

		m_encoder->endEncoding();

		m_commandBuffer->presentDrawable(drawable);
		m_commandBuffer->commit();
	}

	void MetalRenderPassImpl::draw(const gfx::PrimitiveType type, const std::uint32_t start,
								   const uint32_t count)
	{
		m_encoder->drawPrimitives(toMetal(type), start, count);
	}

	void MetalRenderPassImpl::drawIndexed(const gfx::PrimitiveType type, const uint32_t indexCount,
										  const gfx::Buffer& indexBuffer,
										  const uint32_t indexOffset)
	{
		m_encoder->drawIndexedPrimitives(toMetal(type), indexCount, MTL::IndexTypeUInt32,
										 toMetalImpl(indexBuffer)->buffer(), indexOffset);
	}

	void MetalRenderPassImpl::setViewport(const float x, const float y, const float w,
										  const float h, const float zmin, const float zmax)
	{
		m_encoder->setViewport({x, y, w, h, zmin, zmax});
	}

	void MetalRenderPassImpl::setScissor(const uint32_t x, const uint32_t y, const uint32_t w,
										 const uint32_t h)
	{
		m_encoder->setScissorRect({x, y, w, h});
	}

	void MetalRenderPassImpl::waitUntilComplete()
	{
		m_commandBuffer->waitUntilCompleted();
	}

	void MetalRenderPassImpl::setFillMode(const gfx::FillMode fillMode)
	{
		m_encoder->setTriangleFillMode(toMetal(fillMode));
	}
} // namespace lune::metal
