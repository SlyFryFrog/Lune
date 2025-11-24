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

	ComputeKernel& ComputeKernel::dispatch(const size_t threadCount)
	{
		auto* commandBuffer = MetalContext::instance().commandQueue()->commandBuffer();
		auto* encoder = commandBuffer->computeCommandEncoder();

		encoder->setComputePipelineState(m_pipeline.get());

		bindBuffers(encoder);
		bindTextures(encoder);

		NS::UInteger tgSize = m_pipeline->maxTotalThreadsPerThreadgroup();
		NS::UInteger groups = (threadCount + tgSize - 1) / tgSize;

		encoder->dispatchThreadgroups(
			{groups, 1, 1},
			{tgSize, 1, 1}
			);
		encoder->endEncoding();
		commandBuffer->commit();
		m_lastCommandBuffer = NS::TransferPtr(commandBuffer);

		return *this;
	}

	ComputeKernel& ComputeKernel::dispatch(const size_t x, const size_t y, const size_t z)
	{
		auto* commandBuffer = MetalContext::instance().commandQueue()->commandBuffer();
		auto* encoder = commandBuffer->computeCommandEncoder();

		encoder->setComputePipelineState(m_pipeline.get());

		bindBuffers(encoder);
		bindTextures(encoder);

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

		return *this;
	}

	ComputeKernel& ComputeKernel::dispatch(const size_t x, const size_t y, const size_t z,
	                                       std::function<void()> callback)
	{
		auto* commandBuffer = MetalContext::instance().commandQueue()->commandBuffer();
		auto* encoder = commandBuffer->computeCommandEncoder();

		encoder->setComputePipelineState(m_pipeline.get());

		bindBuffers(encoder);
		bindTextures(encoder);

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

		return *this;
	}

	ComputeKernel& ComputeKernel::dispatch(const MTL::Size& threadGroups,
	                                       const MTL::Size& threadsPerGroup)
	{
		// Get a new command buffer
		const auto commandBuffer = NS::TransferPtr(
			MetalContext::instance().commandQueue()->commandBuffer());
		if (!commandBuffer)
			throw std::runtime_error("Failed to create Metal command buffer");

		// Create a compute encoder
		auto* encoder = commandBuffer->computeCommandEncoder();
		if (!encoder)
			throw std::runtime_error("Failed to create Metal compute command encoder");

		// Set pipeline
		encoder->setComputePipelineState(m_pipeline.get());

		bindBuffers(encoder);
		bindTextures(encoder);

		// Dispatch threads
		encoder->dispatchThreadgroups(threadGroups, threadsPerGroup);
		encoder->endEncoding();

		// Commit the command buffer (non-blocking)
		commandBuffer->commit();

		// Store the command buffer in case the user wants to wait
		m_lastCommandBuffer = commandBuffer;

		return *this;
	}

	ComputeKernel& ComputeKernel::setUniform(const std::string& name, const void* data,
	                                       const size_t size, const BufferUsage options)
	{
		// Allocate a small temp buffer for byte data
		const auto device = m_pipeline->device();
		auto temp = device->newBuffer(size, static_cast<MTL::ResourceOptions>(options));

		std::memcpy(temp->contents(), data, size);
		m_buffers[name] = temp;

		return *this;
	}

	ComputeKernel& ComputeKernel::setUniform(const std::string& name,
	                                        MTL::Buffer* buffer)
	{
		m_buffers[name] = buffer;

		return *this;
	}

	ComputeKernel& ComputeKernel::setUniform(const std::string& name, MTL::Texture* texture)
	{
		m_textures[name] = texture;

		return *this;
	}

	MTL::Texture* ComputeKernel::texture(const std::string& name)
	{
		return m_textures[name];
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
			const auto arg = static_cast<MTL::Argument*>(args->object(i));
			const NS::String* nameObj = arg->name();
			std::string name = nameObj ? nameObj->utf8String() : "<null>";
			const NS::UInteger index = arg->index();

			switch (arg->type())
			{
			case MTL::ArgumentTypeBuffer:
				m_bindings[name] = index;
				break;
			case MTL::ArgumentTypeTexture:
				m_textureBindings[name] = index;
				break;
			default:
				break;
			}
		}
	}

	ComputeKernel& ComputeKernel::waitUntilComplete()
	{
		if (m_lastCommandBuffer)
			m_lastCommandBuffer->waitUntilCompleted();

		return *this;
	}

	void ComputeKernel::bufferToTexture(const MTL::Buffer* buffer, const MTL::Texture* texture,
	                                    const NS::UInteger bytesPerRow,
	                                    const MTL::Size& sourceSize, const bool waitUntilComplete)
	{
		const auto cmdBuffer = MetalContext::instance().commandQueue()->commandBuffer();
		const auto blit = cmdBuffer->blitCommandEncoder();

		blit->copyFromBuffer(
			buffer,
			0,
			bytesPerRow,
			0,
			sourceSize,
			texture,
			0,
			0,
			MTL::Origin{0, 0, 0},
			MTL::BlitOptionNone
			);

		blit->endEncoding();
		cmdBuffer->commit();

		if (waitUntilComplete)
			cmdBuffer->waitUntilCompleted();
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

	void ComputeKernel::bindBuffers(MTL::ComputeCommandEncoder* commandEncoder)
	{
		for (auto& [name, buf] : m_buffers)
		{
			const NS::UInteger index = m_bindings[name];
			commandEncoder->setBuffer(buf, 0, index);
		}
	}

	void ComputeKernel::bindTextures(MTL::ComputeCommandEncoder* commandEncoder)
	{
		for (auto& [name, tex] : m_textures)
		{
			const NS::UInteger index = m_textureBindings[name];
			commandEncoder->setTexture(tex, index);
		}
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
			throw std::runtime_error("Error: Kernel with name \"" + name + "\" not found");
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
