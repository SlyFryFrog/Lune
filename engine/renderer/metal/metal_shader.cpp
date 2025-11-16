module;
#include <iostream>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <utility>
module lune;


namespace lune::metal
{
	Shader::Shader(const NS::SharedPtr<MTL::Device>& device) : m_device(device)
	{
	}

	GraphicsShader::GraphicsShader(const NS::SharedPtr<MTL::Device>& device, const std::string& path, const std::string& vertexMain,
	                               const std::string& fragmentMain) :
		Shader(device),
		m_path(path),
		m_vertexMain(vertexMain),
		m_fragmentMain(fragmentMain)
	{
		createLibrary();
		createRenderPipeline();
	}

	void GraphicsShader::createLibrary()
	{
		MTL::Library* library{};
		NS::Error* error;
		if (m_path.ends_with(".metal")) // Runtime-compiled shader
		{
			// Try to load shader from file on disk
			const auto shaderSource = File::read(m_path);
			const auto nsSource = NS::String::string(shaderSource.value_or("").c_str(),
			                                         NS::UTF8StringEncoding);

			library = m_device->newLibrary(nsSource, nullptr, &error);
		}
		else if (m_path.ends_with(".metallib")) // Precompiled shader
		{
			const auto nsPath = NS::String::string(m_path.c_str(), NS::UTF8StringEncoding);
			library = m_device->newLibrary(nsPath, &error);
		}

		m_vertexFunction = NS::TransferPtr(
			library->newFunction(NS::String::string(m_vertexMain.c_str(), NS::UTF8StringEncoding)));
		m_fragmentFunction = NS::TransferPtr(
			library->newFunction(
				NS::String::string(m_fragmentMain.c_str(), NS::UTF8StringEncoding)));

		if (m_vertexFunction.get() == nullptr || m_fragmentFunction.get() == nullptr)
		{
			std::cerr << "Failed to create Metal shader functions\n";
		}

		library->release();
	}

	void GraphicsShader::createRenderPipeline()
	{
		const auto device = MetalContext::instance().device();
		MTL::RenderPipelineDescriptor* pipelineDescriptor =
			MTL::RenderPipelineDescriptor::alloc()->init();
		pipelineDescriptor->setVertexFunction(m_vertexFunction.get());
		pipelineDescriptor->setFragmentFunction(m_fragmentFunction.get());
		pipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(
			MetalContext::instance().createInfo().colorPixelFormat);

		NS::Error* error = nullptr;
		m_pipelineState = NS::TransferPtr(
			device->newRenderPipelineState(pipelineDescriptor, &error)
			);
		if (!m_pipelineState)
		{
			std::cerr << "Failed to create pipeline state: " << error->localizedDescription()->
				utf8String() << "\n";
		}
		pipelineDescriptor->release();
	}
}
