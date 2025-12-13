module;
#include <Metal/Metal.hpp>
#include <iostream>
#include <utility>
module lune.metal;

namespace lune::metal
{
	void bufferToTexture(const gfx::Buffer& buffer, const gfx::Texture& texture,
						 uint32_t bytesPerRow, const uint32_t sourceSize[3],
						 const bool waitUntilComplete)
	{
		const auto cmdBuffer{MetalContextImpl::instance().commandQueue()->commandBuffer()};
		const auto blit{cmdBuffer->blitCommandEncoder()};

		blit->copyFromBuffer(toMetalImpl(buffer)->buffer(), 0, bytesPerRow, 0,
							 MTL::Size{sourceSize[0], sourceSize[1], sourceSize[2]},
							 toMetalImpl(texture)->texture(), 0, 0, MTL::Origin{0, 0, 0},
							 MTL::BlitOptionNone);

		blit->endEncoding();
		cmdBuffer->commit();

		if (waitUntilComplete)
			cmdBuffer->waitUntilCompleted();
	}

	void MetalComputeKernelImpl::dispatch(const size_t threadCount)
	{
		const auto commandBuffer{MetalContextImpl::instance().commandQueue()->commandBuffer()};
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
	}

	void MetalComputeKernelImpl::dispatch(const size_t x, const size_t y, const size_t z)
	{
		const auto commandBuffer{MetalContextImpl::instance().commandQueue()->commandBuffer()};
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
	}

	void MetalComputeKernelImpl::dispatch(const size_t x, const size_t y, const size_t z,
										  std::function<void()> callback)
	{
		const auto commandBuffer{MetalContextImpl::instance().commandQueue()->commandBuffer()};
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
	}


	void MetalComputeKernelImpl::setUniform(const std::string& name, const gfx::Buffer& buffer)
	{
		m_mtlBuffers[name] = toMetalImpl(buffer)->buffer();
	}

	void MetalComputeKernelImpl::setUniform(const std::string& name, const gfx::Texture& texture)
	{
		m_mtlTextures[name] = toMetalImpl(texture)->texture();
	}

	void MetalComputeKernelImpl::setUniform(const std::string& name, const void* data, size_t size)
	{
		// Allocate a small temp buffer for byte data
		auto buffer{m_device->newBuffer(size, MTL::ResourceStorageModeShared)};

		std::memcpy(buffer->contents(), data, size);
		m_mtlBuffers[name] = buffer;
	}

	void MetalComputeKernelImpl::waitUntilComplete()
	{
		m_lastCommandBuffer->waitUntilCompleted();
	}

	void MetalComputeKernelImpl::createPipeline(MTL::Library* library)
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

	KernelReflectionInfo MetalComputeKernelImpl::createKernelReflectionInfo(
			const std::string& name, const MTL::ComputePipelineReflection* reflection)
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
			default:
				break;
			}

			info.arguments.push_back(std::move(a));
		}

		return info;
	}

	void MetalComputeKernelImpl::bindBuffers(MTL::ComputeCommandEncoder* commandEncoder)
	{
		for (auto& [name, buf] : m_mtlBuffers)
		{
			const NS::UInteger index{m_bindings[name]};
			commandEncoder->setBuffer(buf, 0, index);
		}
	}

	void MetalComputeKernelImpl::bindTextures(MTL::ComputeCommandEncoder* commandEncoder)
	{
		for (auto& [name, tex] : m_mtlTextures)
		{
			const NS::UInteger index{m_textureBindings[name]};
			commandEncoder->setTexture(tex, index);
		}
	}

	void MetalComputeShaderImpl::createPipelines()
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
				m_kernels[name] = std::make_unique<gfx::ComputeKernel>(
						std::make_unique<MetalComputeKernelImpl>(m_device, name));
				auto& kernel = *m_kernels[name];
				toMetalImpl(kernel)->createPipeline(m_library.get());
			}
		}
	}
} // namespace lune::metal
