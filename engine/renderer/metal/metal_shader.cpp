module;
#include <iostream>
#include <Metal/Metal.hpp>
#include <utility>
module lune;

namespace lune::metal
{
	Shader::Shader(MTL::Device* device) :
		m_device(device ? device : MetalContext::instance().device())
	{
	}

	NS::SharedPtr<MTL::Library> Shader::createLibrary(const std::string& path) const
	{
		NS::SharedPtr<MTL::Library> library{};
		NS::Error* error{};
		if (path.ends_with(".metal")) // Runtime-compiled shader
		{
			// Try to load shader from file on disk
			const auto shaderSource = File::read(path);
			const auto nsSource = NS::String::string(shaderSource.value_or("").c_str(),
			                                         NS::UTF8StringEncoding);

			library = NS::TransferPtr(m_device->newLibrary(nsSource, nullptr, &error));
		}
		else if (path.ends_with(".metallib")) // Precompiled shader
		{
			const auto nsPath = NS::String::string(path.c_str(), NS::UTF8StringEncoding);
			library = NS::TransferPtr(m_device->newLibrary(nsPath, &error));
		}

		if (error)
		{
			std::cerr << "Failed to create library: " << error->localizedDescription()->cString(
				NS::UTF8StringEncoding) << "\n";
		}

		return library;
	}

	GraphicsShader::GraphicsShader(const GraphicsShaderCreateInfo& createInfo) :
		Shader(createInfo.device),
		m_colorPixelFormat(createInfo.colorPixelFormat),
		m_path(createInfo.path),
		m_vertexMain(createInfo.vertexMain),
		m_fragmentMain(createInfo.fragmentMain)
	{
		loadFunctions();
		createRenderPipeline();
	}

	void GraphicsShader::loadFunctions()
	{
		const NS::SharedPtr<MTL::Library> library = createLibrary(m_path);

		// Load functions from metal library
		m_vertexFunction = NS::TransferPtr(
			library->newFunction(NS::String::string(m_vertexMain.c_str(), NS::UTF8StringEncoding)));
		m_fragmentFunction = NS::TransferPtr(
			library->newFunction(
				NS::String::string(m_fragmentMain.c_str(), NS::UTF8StringEncoding)));

		if (m_vertexFunction.get() == nullptr || m_fragmentFunction.get() == nullptr)
		{
			std::cerr << "Failed to create Metal shader functions\n";
		}
	}

	void GraphicsShader::createRenderPipeline()
	{
		const NS::SharedPtr<MTL::RenderPipelineDescriptor> pipelineDescriptor = NS::TransferPtr(
			MTL::RenderPipelineDescriptor::alloc()->init());
		pipelineDescriptor->setVertexFunction(m_vertexFunction.get());
		pipelineDescriptor->setFragmentFunction(m_fragmentFunction.get());
		pipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(m_colorPixelFormat);

		// User customization
		setupPipelineDescriptor(pipelineDescriptor.get());

		NS::Error* error = nullptr;
		m_pipelineState = NS::TransferPtr(
			m_device->newRenderPipelineState(pipelineDescriptor.get(), &error)
			);
		if (!m_pipelineState)
		{
			std::cerr << "Failed to create pipeline state: " << error->localizedDescription()->
				utf8String() << "\n";
		}
	}
}
