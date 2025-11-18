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
		commandBuffer->waitUntilCompleted();

		return *this;
	}

	ComputeKernel& ComputeKernel::dispatch(const size_t x, const size_t y, const size_t z)
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
		commandBuffer->waitUntilCompleted();

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


	ComputeShader::ComputeShader(const ComputeShaderCreateInfo& info) :
		Shader(info.device),
		m_path(info.path)
	{
		for (const auto& name : info.kernels)
		{
			m_kernels[name] = std::make_unique<ComputeKernel>(m_device, name);
		}

		createPipelines();
	}

	void ComputeShader::createPipelines()
	{
		const auto library = createLibrary(m_path);

		// Create a pipeline for each kernel
		for (auto& [kernelName, kernel] : m_kernels)
		{
			kernel->createPipeline(library.get());
		}
	}
}
