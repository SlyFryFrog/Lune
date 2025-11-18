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
		NS::Error* error;
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

	ComputeShader::ComputeShader(const ComputeShaderCreateInfo& createInfo) :
		Shader(createInfo.device),
		m_path(createInfo.path)
	{
		for (const auto& name : createInfo.kernels)
		{
			Kernel k;
			k.maxThreads = 512;
			m_kernels[name] = std::move(k);
		}

		createPipelines();
	}


	void ComputeShader::createPipelines()
	{
		const auto library = createLibrary(m_path);

		for (auto& [kernelName, kernel] : m_kernels)
		{
			// Load the kernel function
			kernel.function = NS::TransferPtr(
				library->newFunction(NS::String::string(kernelName.c_str(), NS::UTF8StringEncoding))
				);

			// Create pipeline state with reflection
			NS::Error* error = nullptr;
			MTL::ComputePipelineReflection* reflection = nullptr;

			kernel.pipeline = NS::TransferPtr(
				m_device->newComputePipelineState(
					kernel.function.get(),
					MTL::PipelineOptionArgumentInfo,
					&reflection,
					&error
					)
				);
			if (!kernel.pipeline)
			{
				std::cerr << "Failed to create pipeline state for kernel "
					<< kernelName << ": "
					<< error->localizedDescription()->utf8String()
					<< "\n";
				continue;
			}

			kernel.bufferBindings.clear();

			// Automatically populate bufferBindings from reflection
			const NS::Array* args = reflection->arguments();
			for (NS::UInteger i = 0; i < args->count(); ++i)
			{
				auto arg = static_cast<MTL::Argument*>(args->object(i));
				if (arg->type() == MTL::ArgumentTypeBuffer)
				{
					const NS::String* nameObj = arg->name();
					std::string name = nameObj ? nameObj->utf8String() : "<null>";
					const NS::UInteger index = arg->index();

					kernel.bufferBindings[name].second = index;
				}
			}
		}
	}

	void ComputeShader::dispatch(const std::string& kernelName, const size_t threadCount)
	{
		auto it = m_kernels.find(kernelName);
		if (it == m_kernels.end())
		{
			std::cerr << "ComputeShader: kernel not found: " << kernelName << "\n";
			return;
		}

		auto& kernel = it->second;

		auto commandBuffer = NS::TransferPtr(
			MetalContext::instance().device()->newCommandQueue()->commandBuffer());
		auto encoder = NS::TransferPtr(commandBuffer->computeCommandEncoder());

		encoder->setComputePipelineState(kernel.pipeline.get());

		// Automatically bind all buffers in m_boundBuffers
		for (auto& [name, buf] : m_boundBuffers)
		{
			if (kernel.bufferBindings.contains(name))
			{
				encoder->setBuffer(buf.buffer.get(), kernel.bufferBindings[name].first, kernel.bufferBindings[name].second);
			}
		}

		// Threadgroup sizes
		const NS::UInteger tg = std::min(kernel.maxThreads, static_cast<NS::UInteger>(threadCount));
		encoder->dispatchThreads(MTL::Size(threadCount, 1, 1), MTL::Size(tg, 1, 1));

		encoder->endEncoding();
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();
	}
}
