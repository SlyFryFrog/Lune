module;
#include <iostream>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <utility>
module lune;

namespace lune::metal
{
	Shader::Shader(MTL::Device* device) :
		m_device(device ? device : MetalContext::instance().device())
	{
	}

	void Shader::createLibrary(const std::string& path, NS::Error** error)
	{
		m_library = nullptr;

		if (path.ends_with(".metal")) // Runtime-compiled shader
		{
			const auto shaderSource = File::read(path);
			if (!shaderSource.has_value())
			{
				if (error)
					*error = NS::Error::alloc()->init();
				std::cerr << "Shader file not found: " << path << "\n";
				return;
			}

			const auto nsSource = NS::String::string(shaderSource.value().c_str(),
			                                         NS::UTF8StringEncoding);
			m_library = NS::TransferPtr(m_device->newLibrary(nsSource, nullptr, error));
		}
		else if (path.ends_with(".metallib")) // Precompiled shader
		{
			const auto nsPath = NS::String::string(path.c_str(), NS::UTF8StringEncoding);
			m_library = NS::TransferPtr(m_device->newLibrary(nsPath, error));
		}
	}


	GraphicsShader::GraphicsShader(const std::string& path,
	                               const std::string& vsName,
	                               const std::string& fsName,
	                               MTL::Device* device) :
		Shader(device),
		m_path(path),
		m_vertexMainName(vsName),
		m_fragmentMainName(fsName)
	{
		create();
	}

	void GraphicsShader::create()
	{
		NS::Error* error{};
		createLibrary(m_path, &error);

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
				m_vertex = NS::TransferPtr(
					m_library->newFunction(
						NS::String::string(m_vertexMainName.c_str(), NS::UTF8StringEncoding)));
				m_fragment = NS::TransferPtr(
					m_library->newFunction(
						NS::String::string(m_fragmentMainName.c_str(), NS::UTF8StringEncoding)));
			}
		}
	}

	GraphicsPipeline::GraphicsPipeline(const GraphicsShader& shader,
	                                   const GraphicsPipelineDesc& desc) :
		m_shader(&shader),
		m_desc(desc)
	{
		createPipeline();
	}

	void GraphicsPipeline::createPipeline()
	{
		NS::Error* error{};
		MTL::RenderPipelineReflection* reflection{};

		const NS::SharedPtr<MTL::RenderPipelineDescriptor> descriptor = NS::TransferPtr(
			MTL::RenderPipelineDescriptor::alloc()->init());

		descriptor->setVertexFunction(m_shader->vertex());
		descriptor->setFragmentFunction(m_shader->fragment());
		descriptor->colorAttachments()->object(0)->setPixelFormat(toMetal(m_desc.colorFormat));
		descriptor->setDepthAttachmentPixelFormat(toMetal(m_desc.depthFormat));

		// Configure simple depth/blend state from desc
		auto* colorAttachment = descriptor->colorAttachments()->object(0);
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

		const auto depthDesc = NS::TransferPtr(MTL::DepthStencilDescriptor::alloc()->init());
		depthDesc->setDepthWriteEnabled(m_desc.depthWrite);
		depthDesc->setDepthCompareFunction(m_desc.depthCompare);
		m_depthStencilState =
			NS::TransferPtr(m_shader->device()->newDepthStencilState(depthDesc.get()));


		// Create pipeline state with reflection info (request argument info)
		m_state = NS::TransferPtr(m_shader->device()->newRenderPipelineState(
				descriptor.get(),
				MTL::PipelineOptionArgumentInfo,
				&reflection,
				&error)
			);

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

	std::vector<GraphicsPipeline::ArgumentInfo> GraphicsPipeline::parse(const NS::Array* arguments)
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

	Material& Material::setUniform(const std::string& name, const void* data, const size_t size,
	                               const BufferUsage usage)
	{
		if (!m_pipeline)
			return *this;

		auto buffer = NS::TransferPtr(
			m_pipeline->device()->newBuffer(size, toMetal(usage)));
		if (!buffer)
		{
			std::cerr << "Failed to create uniform buffer for '" << name << "'\n";
			return *this;
		}

		// Copy data to the newly created buffer and set as uniform
		std::memcpy(buffer->contents(), data, size);
		m_uniformBuffers[name] = buffer;
		return *this;
	}

	Material& Material::setUniform(const std::string& name, MTL::Texture* texture)
	{
		if (!texture)
			return *this;

		m_textures[name] = NS::RetainPtr(texture);
		return *this;
	}

	Material& Material::setUniform(const std::string& name, MTL::Buffer* buffer)
	{
		if (!buffer)
			return *this;

		m_uniformBuffers[name] = NS::RetainPtr(buffer);
		return *this;
	}

	void Material::bind(MTL::RenderCommandEncoder* encoder) const
	{
		if (!encoder)
			return;

		// Iterate through all vertex and fragment args and set buffers
		for (auto& arg : m_pipeline->vertexArguments())
		{
			if (arg.type == MTL::ArgumentTypeBuffer)
			{
				auto it = m_uniformBuffers.find(arg.name);
				if (it != m_uniformBuffers.end())
				{
					encoder->setVertexBuffer(it->second.get(), 0, arg.index);
				}
			}
			else if (arg.type == MTL::ArgumentTypeTexture)
			{
				auto it = m_textures.find(arg.name);
				if (it != m_textures.end())
				{
					encoder->setVertexTexture(it->second.get(), arg.index);
				}
			}
		}

		for (auto& arg : m_pipeline->fragmentArguments())
		{
			if (arg.type == MTL::ArgumentTypeBuffer)
			{
				auto it = m_uniformBuffers.find(arg.name);
				if (it != m_uniformBuffers.end())
				{
					encoder->setFragmentBuffer(it->second.get(), 0, arg.index);
				}
			}
			else if (arg.type == MTL::ArgumentTypeTexture)
			{
				auto it = m_textures.find(arg.name);
				if (it != m_textures.end())
				{
					encoder->setFragmentTexture(it->second.get(), arg.index);
				}
			}
		}
	}

	RenderPass& RenderPass::bind(const GraphicsPipeline& pipeline)
	{
		m_encoder->setRenderPipelineState(pipeline.state());
		m_encoder->setDepthStencilState(pipeline.depthStencilState());
		m_encoder->setCullMode(pipeline.cullMode());
		m_encoder->setFrontFacingWinding(pipeline.winding());
		return *this;
	}

	RenderPass& RenderPass::bind(const Material& material)
	{
		material.bind(m_encoder.get());
		return *this;
	}

	void RenderPass::begin(const CA::MetalDrawable* drawable)
	{
		if (!drawable)
		{
			std::cerr << "RenderPass::begin(): drawable is null\n";
			return;
		}

		m_commandBuffer = NS::TransferPtr(
			MetalContext::instance().commandQueue()->commandBuffer());

		const NS::SharedPtr<MTL::RenderPassDescriptor> renderPassDescriptor =
			NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
		MTL::RenderPassColorAttachmentDescriptor* colorAttachmentDescriptor =
			renderPassDescriptor->colorAttachments()->object(0);
		colorAttachmentDescriptor->setTexture(drawable->texture());
		colorAttachmentDescriptor->setLoadAction(MTL::LoadActionLoad);
		colorAttachmentDescriptor->setClearColor(MTL::ClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		colorAttachmentDescriptor->setStoreAction(MTL::StoreActionStore);

		m_encoder = NS::TransferPtr(
			m_commandBuffer->renderCommandEncoder(renderPassDescriptor.get()));
	}

	void RenderPass::end(const CA::MetalDrawable* drawable) const
	{
		m_encoder->endEncoding();

		m_commandBuffer->presentDrawable(drawable);
		m_commandBuffer->commit();
	}

	RenderPass& RenderPass::draw(const PrimitiveType type,
	                             const uint startVertex, const uint vertexCount)
	{
		m_encoder->drawPrimitives(toMetal(type), startVertex, vertexCount);
		return *this;
	}

	RenderPass& RenderPass::setFillMode(const FillMode mode)
	{
		m_encoder->setTriangleFillMode(toMetal(mode));
		return *this;
	}

	RenderPass& RenderPass::waitUntilComplete()
	{
		m_commandBuffer->waitUntilCompleted();
		return *this;
	}
}
