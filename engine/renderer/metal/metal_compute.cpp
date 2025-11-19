module;
#include <iostream>
#include <Metal/Metal.hpp>
#include <utility>
module lune;

namespace lune::metal
{
	ComputeKernel::ComputeKernel(MTL::Device* device, const std::string& name) :
		m_device(device),
		m_name(name)
	{
	}

	ComputeKernel& ComputeKernel::dispatch(const size_t threadCount, const bool async)
	{
		auto* commandBuffer = MetalContext::instance().commandQueue()->commandBuffer();
		auto* encoder = commandBuffer->computeCommandEncoder();

		encoder->setComputePipelineState(m_pipeline.get());

		// Bind buffers
		for (auto& [name, buf] : m_buffers)
		{
			const NS::UInteger index = m_bindings[name];
			encoder->setBuffer(buf.get(), 0, index);
		}

		NS::UInteger tgSize = m_pipeline->maxTotalThreadsPerThreadgroup();
		NS::UInteger groups = (threadCount + tgSize - 1) / tgSize;

		encoder->dispatchThreadgroups({groups, 1, 1}, {tgSize, 1, 1});
		encoder->endEncoding();
		commandBuffer->commit();
		m_lastCommandBuffer = NS::TransferPtr(commandBuffer);

		if (!async)
			commandBuffer->waitUntilCompleted();

		return *this;
	}

	ComputeKernel& ComputeKernel::dispatch(const size_t x, const size_t y, const size_t z,
	                                       const bool async)
	{
		auto* commandBuffer = MetalContext::instance().commandQueue()->commandBuffer();
		auto* encoder = commandBuffer->computeCommandEncoder();

		encoder->setComputePipelineState(m_pipeline.get());

		for (auto& [name, buf] : m_buffers)
		{
			const NS::UInteger index = m_bindings[name];
			encoder->setBuffer(buf.get(), 0, index);
		}

		const MTL::Size threadsPerGroup = {16, 16, 1};
		const MTL::Size groups = {
			(x + 15) / 16,
			(y + 15) / 16,
			z
		};

		encoder->dispatchThreadgroups(groups, threadsPerGroup);
		encoder->endEncoding();
		commandBuffer->commit();
		m_lastCommandBuffer = NS::TransferPtr(commandBuffer);

		if (!async)
			commandBuffer->waitUntilCompleted();

		return *this;
	}

	ComputeKernel& ComputeKernel::dispatch(const size_t x, const size_t y, const size_t z,
	                                       std::function<void()> callback, const bool async)
	{
		auto* commandBuffer = MetalContext::instance().commandQueue()->commandBuffer();
		auto* encoder = commandBuffer->computeCommandEncoder();

		encoder->setComputePipelineState(m_pipeline.get());

		for (auto& [name, buf] : m_buffers)
		{
			const NS::UInteger index = m_bindings[name];
			encoder->setBuffer(buf.get(), 0, index);
		}

		const MTL::Size threadsPerGroup = {16, 16, 1};
		const MTL::Size groups = {
			(x + 15) / 16,
			(y + 15) / 16,
			z
		};

		encoder->dispatchThreadgroups(groups, threadsPerGroup);
		encoder->endEncoding();
		commandBuffer->addCompletedHandler(
			[callback = std::move(callback)](MTL::CommandBuffer*)
			{
				callback();
			});
		commandBuffer->commit();
		m_lastCommandBuffer = NS::TransferPtr(commandBuffer);

		if (!async)
			commandBuffer->waitUntilCompleted();

		return *this;
	}

	ComputeKernel& ComputeKernel::setBuffer(const std::string& name,
	                                        const NS::SharedPtr<MTL::Buffer>& buf)
	{
		m_buffers[name] = buf;
		return *this;
	}

	ComputeKernel& ComputeKernel::setBytes(const std::string& name, const void* data,
	                                       const size_t size)
	{
		// Allocate a small temp buffer for byte data
		const auto device = m_pipeline->device();
		NS::SharedPtr<MTL::Buffer> temp =
			NS::TransferPtr(device->newBuffer(size, MTL::ResourceStorageModeShared));

		std::memcpy(temp->contents(), data, size);
		m_buffers[name] = temp;

		return *this;
	}

	void ComputeKernel::createPipeline(MTL::Library* library)
	{
		// Load the kernel function
		m_function = NS::TransferPtr(
			library->newFunction(NS::String::string(m_name.c_str(), NS::UTF8StringEncoding))
			);

		// Create pipeline state with reflection
		NS::Error* error = nullptr;
		MTL::ComputePipelineReflection* reflection = nullptr;

		// Create the pipeline
		m_pipeline = NS::TransferPtr(
			m_device->newComputePipelineState(
				m_function.get(),
				MTL::PipelineOptionArgumentInfo,
				&reflection,
				&error
				)
			);
		if (error)
		{
			std::cerr << "Failed to create pipeline state for kernel "
				<< m_name << ": "
				<< error->localizedDescription()->utf8String()
				<< "\n";
			return;
		}

		m_reflection = createKernelReflectionInfo(m_name, reflection);

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

				m_bindings[name] = index;
			}
		}
	}

	ComputeKernel& ComputeKernel::waitUntilComplete()
	{
		if (m_lastCommandBuffer)
			m_lastCommandBuffer->waitUntilCompleted();

		return *this;
	}

	KernelReflectionInfo ComputeKernel::createKernelReflectionInfo(
		const std::string& name,
		const MTL::ComputePipelineReflection* reflection)
	{
		KernelReflectionInfo info{};
		info.kernelName = name;

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

	ComputeShader::ComputeShader(const std::string& path, MTL::Device* device) :
		Shader(device),
		m_path(path)

	{
		createPipelines();
	}

	ComputeKernel& ComputeShader::kernel(const std::string& name)
	{
		if (!m_kernels.contains(name))
		{
			throw std::runtime_error("Kernel name not found");
		}

		return *m_kernels[name];
	}

	std::vector<std::string> ComputeShader::listKernels() const
	{
		std::vector<std::string> names;
		for (const auto& [name, kernel] : m_kernels)
		{
			names.push_back(name);
		}

		return names;
	}

	void ComputeShader::createPipelines()
	{
		const NS::SharedPtr<MTL::Library> library = createLibrary(m_path);
		const NS::Array* functionNames = library->functionNames();

		for (int i = 0; i < functionNames->count(); ++i)
		{
			NS::String* nsName = functionNames->object<NS::String>(i);
			std::string name = functionNames->object<NS::String>(i)->utf8String();
			NS::SharedPtr<MTL::Function> function = NS::TransferPtr(library->newFunction(nsName));
			if (!function)
				continue;

			if (function->functionType() == MTL::FunctionTypeKernel)
			{
				m_kernels[name] = std::make_unique<ComputeKernel>(m_device, name);
				m_kernels[name]->createPipeline(library.get());
			}
		}
	}

	void printKernelInfo(const KernelReflectionInfo& info)
	{
		std::cout << "================= Kernel Reflection =================\n";
		std::cout << "Function: " << info.kernelName << "\n\n";

		if (!info.arguments.empty())
		{
			std::cout << "Arguments (" << info.arguments.size() << "):\n";
			for (const auto& arg : info.arguments)
			{
				std::cout
					<< "  - Name      : " << arg.name << "\n"
					<< "    Index     : " << arg.index << "\n"
					<< "    Type      : " << to_string(arg.type) << "\n"
					<< "    Data Type : " << to_string(arg.dataType) << "\n"
					<< "    Data Size : " << arg.dataSize << " bytes\n\n";
			}
		}

		if (info.threadgroupMemory.has_value())
		{
			std::cout << "Threadgroup Memory:\n";
			for (const auto& tg : info.threadgroupMemory.value())
			{
				std::cout
					<< "  - Index     : " << tg.index << "\n"
					<< "    Size      : " << tg.size << " bytes\n"
					<< "    Alignment : " << tg.alignment << " bytes\n\n";
			}
			std::cout << "\n";
		}

		std::cout << "=====================================================\n\n";
	}
}
