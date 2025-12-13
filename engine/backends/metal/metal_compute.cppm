module;
#include <Metal/Metal.hpp>
#include <map>
#include <string>
export module lune.metal:compute;

import :mappings;
import lune.gfx;

namespace lune::metal
{
	export void bufferToTexture(const gfx::Buffer& buffer, const gfx::Texture& texture,
								uint32_t bytesPerRow, const uint32_t sourceSize[3],
								bool waitUntilComplete);

	export class MetalComputeKernelImpl : public gfx::IComputeKernelImpl
	{
		struct ArgumentInfo
		{
			std::string name;
			uint32_t index;
			uint32_t arrayLength;
			MTL::ArgumentType type;
		};

		NS::SharedPtr<MTL::CommandBuffer> m_lastCommandBuffer;
		NS::SharedPtr<MTL::ComputePipelineState> m_pipeline;
		NS::SharedPtr<MTL::Function> m_function;
		MTL::Device* m_device{};

		std::map<std::string, NS::UInteger> m_bindings{};
		std::map<std::string, NS::UInteger> m_textureBindings{};
		std::map<std::string, MTL::Buffer*> m_mtlBuffers;
		std::map<std::string, MTL::Texture*> m_mtlTextures;

		std::vector<ArgumentInfo> m_computeArguments{};

	public:
		MetalComputeKernelImpl(MTL::Device* device, const std::string& name) :
			IComputeKernelImpl(name), m_device(device)
		{
		}

		~MetalComputeKernelImpl() override = default;

		void dispatch(size_t threadCount) override;
		void dispatch(size_t x, size_t y, size_t z) override;
		void dispatch(size_t x, size_t y, size_t z, std::function<void()> callback) override;

		void setUniform(const std::string& name, const gfx::Buffer& buffer) override;
		void setUniform(const std::string& name, const gfx::Texture& texture) override;
		void setUniform(const std::string& name, const void* data, size_t size) override;

		void waitUntilComplete() override;

		void createPipeline(MTL::Library* library);

	private:
		[[nodiscard]] static std::vector<ArgumentInfo>
		getComputeArguments(const MTL::ComputePipelineReflection* reflection);

		void bindBuffers(MTL::ComputeCommandEncoder* commandEncoder);
		void bindTextures(MTL::ComputeCommandEncoder* commandEncoder);
	};


	export class MetalComputeShaderImpl : public gfx::IComputeShaderImpl
	{
		MTL::Device* m_device{};
		NS::SharedPtr<MTL::Library> m_library{};

	public:
		MetalComputeShaderImpl(MTL::Device* device, const std::string& path) :
			IComputeShaderImpl(path), m_device(device)
		{
			createPipelines();
		}

		~MetalComputeShaderImpl() override = default;

	private:
		void createPipelines();
	};


	constexpr MetalComputeKernelImpl* toMetalImpl(const gfx::ComputeKernel& kernel)
	{
		const auto impl = kernel.getImpl();

#ifndef NDEBUG
		const auto metalImpl = dynamic_cast<MetalComputeKernelImpl*>(impl);
		if (!metalImpl)
			throw std::runtime_error("Kernel is not a Metal compute kernel!");
		return metalImpl;
#else
		return static_cast<MetalComputeKernelImpl*>(impl);
#endif
	}


	constexpr MetalComputeShaderImpl* toMetalImpl(const gfx::ComputeShader& shader)
	{
		const auto impl{shader.getImpl()};

#ifndef NDEBUG
		const auto metalImpl{dynamic_cast<MetalComputeShaderImpl*>(impl)};
		if (!metalImpl)
			throw std::runtime_error("Shader is not a Metal compute shader!");
		return metalImpl;
#else
		return static_cast<MetalComputeShaderImpl*>(impl);
#endif
	}
} // namespace lune::metal
