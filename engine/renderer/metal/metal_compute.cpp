module;
#include <iostream>
#include <Metal/Metal.hpp>
#include <utility>
module lune;

namespace lune::metal
{
	ComputeShader::ComputeShader(const ComputeShaderCreateInfo& createInfo) :
		Shader(createInfo.device),
		m_path(createInfo.path)
	{
		for (const auto& name : createInfo.kernels)
		{
			Kernel kernel;
			m_kernels[name] = std::move(kernel);
		}

		createPipelines();
	}

	ComputeShader& ComputeShader::dispatch(const std::string& kernelName, const size_t threadCount)
	{
		const auto it = m_kernels.find(kernelName);
		if (it == m_kernels.end())
		{
			std::cerr << "ComputeShader: kernel not found: " << kernelName << "\n";
			return *this;
		}

		auto& kernel = it->second; // Extract kernel from pair for ease of use

		const NS::SharedPtr<MTL::CommandBuffer> commandBuffer = NS::TransferPtr(
			MetalContext::instance().device()->newCommandQueue()->commandBuffer());
		const NS::SharedPtr<MTL::ComputeCommandEncoder> encoder = NS::TransferPtr(
			commandBuffer->computeCommandEncoder());

		encoder->setComputePipelineState(kernel.pipeline.get());

		// Automatically bind all buffers in m_boundBuffers
		for (auto& [name, buf] : m_boundBuffers)
		{
			if (kernel.bufferBindings.contains(name))
			{
				encoder->setBuffer(buf.buffer.get(), 0, kernel.bufferBindings[name]);
			}
		}

		// Threadgroup sizes
		const NS::UInteger tg = std::min(kernel.pipeline->maxTotalThreadsPerThreadgroup(),
		                                 static_cast<NS::UInteger>(threadCount));
		encoder->dispatchThreads(MTL::Size(threadCount, 1, 1), MTL::Size(tg, 1, 1));

		encoder->endEncoding();
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();

		return *this;
	}

	KernelReflectionInfo ComputeShader::kernelReflection(std::string_view name)
	{
		auto it = m_kernels.find(std::string(name));
		if (it == m_kernels.end())
		{
			std::cerr << "ComputeShader: kernel not found: " << name << "\n";
			return {};
		}

		const auto& kernel = it->second;

		// Build a temporary pipeline to fetch reflection (in case pipeline is not built yet)
		NS::Error* error = nullptr;
		MTL::ComputePipelineReflection* reflection = nullptr;

		const NS::SharedPtr<MTL::ComputePipelineState> pipeline = NS::TransferPtr(
			m_device->newComputePipelineState(
				kernel.function.get(),
				MTL::PipelineOptionArgumentInfo,
				&reflection,
				&error
				)
			);
		if (error)
		{
			std::cerr << "Reflection failed for kernel " << name << ": "
				<< error->localizedDescription()->utf8String() << "\n";
			return {};
		}

		return createKernelReflectionInfo(name, reflection);
	}

	std::vector<std::string> ComputeShader::kernelNames() const
	{
		std::vector<std::string> names;
		names.reserve(m_kernels.size());

		for(const auto& [name, kernel] : m_kernels)
		{
			names.push_back(name);
		}

		return names;
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

			// Create the pipeline
			kernel.pipeline = NS::TransferPtr(
				m_device->newComputePipelineState(
					kernel.function.get(),
					MTL::PipelineOptionArgumentInfo,
					&reflection,
					&error
					)
				);
			if (error)
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
				if (const auto arg = static_cast<MTL::Argument*>(args->object(i));
					arg->type() == MTL::ArgumentTypeBuffer)
				{
					const NS::String* nameObj = arg->name();
					std::string name = nameObj ? nameObj->utf8String() : "<null>";
					const NS::UInteger index = arg->index();

					kernel.bufferBindings[name] = index;
				}
			}
		}
	}

	KernelReflectionInfo ComputeShader::createKernelReflectionInfo(
		const std::string_view name,
		const MTL::ComputePipelineReflection*
		reflection)
	{
		KernelReflectionInfo info{};
		info.kernelName = std::string(name);

		const NS::Array* args = reflection->arguments();
		info.arguments.reserve(args->count());

		// Iterate through each argument and add all relevant information struct
		for (NS::UInteger i = 0; i < args->count(); ++i)
		{
			const auto arg = static_cast<MTL::Argument*>(args->object(i));
			KernelReflectionInfo::KernelArgumentInfo a{};

			a.name = arg->name() ? arg->name()->utf8String() : "";
			a.index = arg->index();
			a.type = arg->type();
			a.access = arg->access();

			switch (arg->type())
			{
			case MTL::ArgumentTypeBuffer:
				a.dataType = arg->bufferDataType();
				a.dataSize = arg->bufferDataSize();
				a.alignment = arg->bufferAlignment();
				a.structType = arg->bufferStructType();
				break;
			case MTL::ArgumentTypeTexture:
				a.textureType = arg->textureType();
				a.textureDataType = arg->textureDataType();
				a.isDepthTexture = arg->isDepthTexture();
				break;
			case MTL::ArgumentTypeThreadgroupMemory:
			{
				KernelReflectionInfo::ThreadgroupMem mem{};
				mem.index = arg->index();
				mem.size = arg->threadgroupMemoryDataSize();
				mem.alignment = arg->threadgroupMemoryAlignment();

				info.threadgroupMemory->push_back(mem);
				break;
			}
			default:
				break;
			}

			info.arguments.push_back(std::move(a));
		}

		return info;
	}
}
