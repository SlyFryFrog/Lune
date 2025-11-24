module;
#include <iostream>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <utility>
module lune;


namespace lune::metal
{
	GraphicsShader::GraphicsShader(const std::string& path,
	                           const std::string& vsName,
	                           const std::string& fsName,
	                           MTL::Device* device) :
		m_device(device ? device : MetalContext::instance().device())
	{
		NS::Error* error = nullptr;
		createLibrary(path, &error);

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
						NS::String::string(vsName.c_str(), NS::UTF8StringEncoding)));
				m_fragment = NS::TransferPtr(
					m_library->newFunction(
						NS::String::string(fsName.c_str(), NS::UTF8StringEncoding)));
			}
		}
	}

	void GraphicsShader::createLibrary(const std::string& path, NS::Error** error)
	{
		// Defensive: clear previous library
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
		else
		{
			// Unknown extension — try to load as text first, then as metallib path
			if (const auto shaderSource = File::read(path); shaderSource.has_value())
			{
				const auto nsSource = NS::String::string(shaderSource.value().c_str(),
				                                         NS::UTF8StringEncoding);
				m_library = NS::TransferPtr(m_device->newLibrary(nsSource, nullptr, error));
			}
			else
			{
				const auto nsPath = NS::String::string(path.c_str(), NS::UTF8StringEncoding);
				m_library = NS::TransferPtr(m_device->newLibrary(nsPath, error));
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
		NS::Error* error = nullptr;
		MTL::RenderPipelineReflection* reflection = nullptr;

		const NS::SharedPtr<MTL::RenderPipelineDescriptor> descriptor = NS::TransferPtr(
			MTL::RenderPipelineDescriptor::alloc()->init());

		descriptor->setVertexFunction(m_shader->vertex());
		descriptor->setFragmentFunction(m_shader->fragment());
		descriptor->colorAttachments()->object(0)->setPixelFormat(m_desc.colorFormat);
		descriptor->setDepthAttachmentPixelFormat(m_desc.depthFormat);

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
		}

		// Build reflection (may be nullptr if pipeline creation failed or didn't produce reflection)
		m_reflection = buildReflection(reflection);
	}

	GraphicsReflectionInfo GraphicsPipeline::buildReflection(
		const MTL::RenderPipelineReflection* reflection)
	{
		GraphicsReflectionInfo info{};

		if (!reflection)
			return info;

		const NS::Array* fragArgs = reflection->fragmentArguments();
		const NS::Array* vertArgs = reflection->vertexArguments();

		// Todo

		return info;
	}

	void Material::setUniform(const std::string& name, const void* data, size_t size)
	{
		if (!m_pipeline)
			return;

		auto buffer = NS::TransferPtr(
			m_pipeline->device()->newBuffer(size, MTL::ResourceStorageModeShared));
		if (!buffer)
		{
			std::cerr << "Failed to create uniform buffer for '" << name << "'\n";
			return;
		}

		// Copy data to the newly created buffer and set as uniform
		std::memcpy(buffer->contents(), data, size);
		m_uniformBuffers[name] = buffer;
	}

	void Material::setUniform(const std::string& name, MTL::Texture* texture)
	{
		m_textures[name] = texture;
	}

	void Material::bind(MTL::RenderCommandEncoder* encoder) const
	{
		if (!encoder)
			return;

		// Simple, deterministic binding: bind buffers and textures sequentially
		// to both vertex and fragment stages starting at slot 0.
		uint index = 0;
		for (const auto& [name, buf] : m_uniformBuffers)
		{
			if (buf)
			{
				encoder->setVertexBuffer(buf.get(), 0, index);
				encoder->setFragmentBuffer(buf.get(), 0, index);
			}
			++index;
		}

		index = 0;
		for (const auto& [name, tex] : m_textures)
		{
			if (tex)
			{
				encoder->setFragmentTexture(tex, index);
				encoder->setVertexTexture(tex, index);
			}
			++index;
		}
	}

	void RenderPass::begin(const CA::MetalDrawable* drawable)
	{
		m_commandBuffer = NS::TransferPtr(
			MetalContext::instance().commandQueue()->commandBuffer());

		const NS::SharedPtr<MTL::RenderPassDescriptor> renderPassDescriptor =
			NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
		MTL::RenderPassColorAttachmentDescriptor* colorAttachmentDescriptor =
			renderPassDescriptor->colorAttachments()->object(0);
		colorAttachmentDescriptor->setTexture(drawable->texture());
		colorAttachmentDescriptor->setLoadAction(MTL::LoadActionClear);
		colorAttachmentDescriptor->setClearColor(MTL::ClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		colorAttachmentDescriptor->setStoreAction(MTL::StoreActionStore);

		m_encoder = m_commandBuffer->renderCommandEncoder(renderPassDescriptor.get());

	}

	void RenderPass::end(const CA::MetalDrawable* drawable) const
	{
		m_encoder->endEncoding();

		m_commandBuffer->presentDrawable(drawable);
		m_commandBuffer->commit();
		m_commandBuffer->waitUntilCompleted();
	}

	void RenderPass::draw(const MTL::PrimitiveType type, const uint vertexCount,
	                      const uint startVertex) const
	{
		m_encoder->drawPrimitives(type, startVertex, vertexCount);
	}


	Shader::Shader(MTL::Device* device) :
		m_device(device ? device : MetalContext::instance().device())
	{
	}

	NS::SharedPtr<MTL::Library> Shader::createLibrary(const std::string& path) const
	{
		NS::SharedPtr<MTL::Library> library{};
		NS::Error* error = nullptr;

		if (path.ends_with(".metal"))
		{
			const auto shaderSource = File::read(path);
			if (!shaderSource.has_value())
			{
				std::cerr << "Shader file not found: " << path << "\n";
				return library;
			}

			const auto nsSource = NS::String::string(shaderSource.value().c_str(),
			                                         NS::UTF8StringEncoding);
			library = NS::TransferPtr(m_device->newLibrary(nsSource, nullptr, &error));
		}
		else if (path.ends_with(".metallib"))
		{
			const auto nsPath = NS::String::string(path.c_str(), NS::UTF8StringEncoding);
			library = NS::TransferPtr(m_device->newLibrary(nsPath, &error));
		}
		else
		{
			// Try both approaches
			const auto shaderSource = File::read(path);
			if (shaderSource.has_value())
			{
				const auto nsSource = NS::String::string(shaderSource.value().c_str(),
				                                         NS::UTF8StringEncoding);
				library = NS::TransferPtr(m_device->newLibrary(nsSource, nullptr, &error));
			}
			else
			{
				const auto nsPath = NS::String::string(path.c_str(), NS::UTF8StringEncoding);
				library = NS::TransferPtr(m_device->newLibrary(nsPath, &error));
			}
		}

		if (error)
		{
			auto desc = error->localizedDescription();
			if (desc)
				std::cerr << "Failed to create library: " << desc->cString(NS::UTF8StringEncoding)
					<< "\n";
			else
				std::cerr << "Failed to create library: unknown error\n";
		}

		return library;
	}
}
