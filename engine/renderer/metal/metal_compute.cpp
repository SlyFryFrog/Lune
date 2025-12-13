module;
#include <Metal/Metal.hpp>
#include <iostream>
#include <utility>
module lune.metal;

namespace lune::metal
{
	ComputeKernel::ComputeKernel(MTL::Device* device, const std::string& name) :
		m_device(device), m_name(name)
	{
	}

	ComputeKernel& ComputeKernel::dispatch(const size_t threadCount)
	{
		const auto commandBuffer{MetalContext::instance().commandQueue()->commandBuffer()};
		const auto encoder{commandBuffer->computeCommandEncoder()};

		encoder->setComputePipelineState(m_pipeline.get());

		bindBuffers(encoder);
		bindTextures(encoder);

		NS::UInteger tgSize{m_pipeline->maxTotalThreadsPerThreadgroup()};
		NS::UInteger groups{(threadCount + tgSize - 1) / tgSize};

		encoder->dispatchThreadgroups({groups, 1, 1}, {tgSize, 1, 1});
		encoder->endEncoding();
		commandBuffer->commit();
		m_lastCommandBuffer = NS::TransferPtr(commandBuffer);

		return *this;
	}

	ComputeKernel& ComputeKernel::dispatch(const size_t x, const size_t y, const size_t z)
	{
		const auto commandBuffer{MetalContext::instance().commandQueue()->commandBuffer()};
		const auto encoder{commandBuffer->computeCommandEncoder()};

		encoder->setComputePipelineState(m_pipeline.get());

		bindBuffers(encoder);
		bindTextures(encoder);

		const MTL::Size threadsPerGroup{16, 16, 1};
		const MTL::Size groups{(x + 15) / 16, (y + 15) / 16, z};

		encoder->dispatchThreadgroups(groups, threadsPerGroup);
		encoder->endEncoding();
		commandBuffer->commit();
		m_lastCommandBuffer = NS::TransferPtr(commandBuffer);

		return *this;
	}

	ComputeKernel& ComputeKernel::dispatch(const size_t x, const size_t y, const size_t z,
										   std::function<void()> callback)
	{
		const auto commandBuffer{MetalContext::instance().commandQueue()->commandBuffer()};
		const auto encoder{commandBuffer->computeCommandEncoder()};

		encoder->setComputePipelineState(m_pipeline.get());

		bindBuffers(encoder);
		bindTextures(encoder);

		const MTL::Size threadsPerGroup{16, 16, 1};
		const MTL::Size groups{(x + 15) / 16, (y + 15) / 16, z};

		encoder->dispatchThreadgroups(groups, threadsPerGroup);
		encoder->endEncoding();
		commandBuffer->addCompletedHandler([callback = std::move(callback)](MTL::CommandBuffer*)
										   { callback(); });
		commandBuffer->commit();
		m_lastCommandBuffer = NS::TransferPtr(commandBuffer);

		return *this;
	}

	ComputeKernel& ComputeKernel::dispatch(const MTL::Size& threadGroups,
										   const MTL::Size& threadsPerGroup)
	{
		const auto commandBuffer{
				NS::TransferPtr(MetalContext::instance().commandQueue()->commandBuffer())};
		if (!commandBuffer)
			throw std::runtime_error("Failed to create Metal command buffer");

		const auto encoder{commandBuffer->computeCommandEncoder()};
		if (!encoder)
			throw std::runtime_error("Failed to create Metal compute command encoder");

		encoder->setComputePipelineState(m_pipeline.get());

		bindBuffers(encoder);
		bindTextures(encoder);

		encoder->dispatchThreadgroups(threadGroups, threadsPerGroup);
		encoder->endEncoding();

		commandBuffer->commit();

		m_lastCommandBuffer = commandBuffer;

		return *this;
	}

	ComputeKernel& ComputeKernel::setUniform(const std::string& name, const gfx::Buffer& buffer)
	{
		m_mtlBuffers[name] = toMetalImpl(buffer)->buffer();
		return *this;
	}

	ComputeKernel& ComputeKernel::setUniform(const std::string& name, const gfx::Texture& texture)
	{
		m_mtlTextures[name] = toMetalImpl(texture)->texture();
		return *this;
	}

	ComputeKernel& ComputeKernel::setUniform(const std::string& name, const void* data,
											 const size_t size, const gfx::BufferUsage usage)
	{
		// Allocate a small temp buffer for byte data
		const auto device{m_pipeline->device()};
		auto temp{device->newBuffer(size, toMetal(usage))};

		std::memcpy(temp->contents(), data, size);
		m_mtlBuffers[name] = temp;

		return *this;
	}

	void ComputeKernel::createPipeline(MTL::Library* library)
	{
		// Load the kernel function
		m_function = NS::TransferPtr(
				library->newFunction(NS::String::string(m_name.c_str(), NS::UTF8StringEncoding)));

		// Create pipeline state with reflection
		NS::Error* error{};
		MTL::ComputePipelineReflection* reflection{};

		// Create the pipeline
		m_pipeline = NS::TransferPtr(m_device->newComputePipelineState(
				m_function.get(), MTL::PipelineOptionArgumentInfo, &reflection, &error));
		if (error)
		{
			std::cerr << "Failed to create pipeline state for kernel " << m_name << ": "
					  << error->localizedDescription()->utf8String() << "\n";
			return;
		}

		m_reflection = createKernelReflectionInfo(m_name, reflection);

		// Automatically populate bufferBindings from reflection
		const NS::Array* args{reflection->arguments()};
		for (NS::UInteger i = 0; i < args->count(); ++i)
		{
			const auto arg{static_cast<MTL::Argument*>(args->object(i))};
			const NS::String* nameObj{arg->name()};
			std::string name{nameObj ? nameObj->utf8String() : "<null>"};
			const NS::UInteger index{arg->index()};

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

	void ComputeKernel::bufferToTexture(const gfx::Buffer& buffer, const gfx::Texture& texture,
										const NS::UInteger bytesPerRow, const MTL::Size& sourceSize,
										const bool waitUntilComplete)
	{
		const auto cmdBuffer{MetalContext::instance().commandQueue()->commandBuffer()};
		const auto blit{cmdBuffer->blitCommandEncoder()};

		blit->copyFromBuffer(toMetalImpl(buffer)->buffer(), 0, bytesPerRow, 0, sourceSize,
							 toMetalImpl(texture)->texture(), 0, 0, MTL::Origin{0, 0, 0},
							 MTL::BlitOptionNone);

		blit->endEncoding();
		cmdBuffer->commit();

		if (waitUntilComplete)
			cmdBuffer->waitUntilCompleted();
	}

	KernelReflectionInfo
	ComputeKernel::createKernelReflectionInfo(const std::string& name,
											  const MTL::ComputePipelineReflection* reflection)
	{
		KernelReflectionInfo info{};
		info.kernelName = name;

		const NS::Array* args{reflection->arguments()};
		info.arguments.reserve(args->count());

		// Iterate through each argument and add all relevant information struct
		for (NS::UInteger i = 0; i < args->count(); ++i)
		{
			const auto arg = reinterpret_cast<MTL::Argument*>(args->object(i));
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
		for (auto& [name, buf] : m_mtlBuffers)
		{
			const NS::UInteger index{m_bindings[name]};
			commandEncoder->setBuffer(buf, 0, index);
		}
	}

	void ComputeKernel::bindTextures(MTL::ComputeCommandEncoder* commandEncoder)
	{
		for (auto& [name, tex] : m_mtlTextures)
		{
			const NS::UInteger index{m_textureBindings[name]};
			commandEncoder->setTexture(tex, index);
		}
	}

	ComputeShader::ComputeShader(const std::string& path, MTL::Device* device) :
		m_device(device ? device : MetalContext::instance().device()), m_path(path)

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
		NS::Error* error{};
		m_library = NS::TransferPtr(createLibrary(m_path, m_device, &error));

		if (error || !m_library)
		{
			if (const auto desc = error->localizedDescription())
				std::cerr << "Failed to create library: " << desc->cString(NS::UTF8StringEncoding)
						  << "\n";
			else
				std::cerr << "Failed to create library: unknown error\n";

			return;
		}

		const NS::Array* functionNames{m_library->functionNames()};

		for (int i = 0; i < functionNames->count(); ++i)
		{
			NS::String* nsName{functionNames->object<NS::String>(i)};
			std::string name{functionNames->object<NS::String>(i)->utf8String()};
			NS::SharedPtr<MTL::Function> function{NS::TransferPtr(m_library->newFunction(nsName))};
			if (!function)
				continue;

			if (function->functionType() == MTL::FunctionTypeKernel)
			{
				m_kernels[name] = std::make_unique<ComputeKernel>(m_device, name);
				m_kernels[name]->createPipeline(m_library.get());
			}
		}
	}
} // namespace lune::metal
