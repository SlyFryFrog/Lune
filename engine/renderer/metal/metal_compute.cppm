module;
#include <iostream>
#include <string>
#include <string_view>
#include <Metal/Metal.hpp>
#include <map>
#include <span>
export module lune:metal_compute;

import :metal_shader;

namespace lune::metal
{
	export struct ComputeShaderCreateInfo
	{
		MTL::Device* device = nullptr;
		MTL::PipelineOption pipelineOption = MTL::PipelineOptionNone;
		MTL::AutoreleasedComputePipelineReflection* reflection = nullptr;
		std::string path;
		std::vector<std::string> kernels;
	};


	export struct KernelReflectionInfo
	{
		std::string kernelName;
		MTL::Function* function;

		struct KernelArgumentInfo
		{
			std::string name;
			NS::UInteger index;

			MTL::ArgumentType type;
			MTL::BindingAccess access;

			MTL::DataType dataType{};
			size_t dataSize{};
			size_t alignment{};
			const MTL::StructType* structType{};

			MTL::TextureType textureType;
			MTL::DataType textureDataType;
			bool isDepthTexture;
		};

		std::vector<KernelArgumentInfo> arguments;

		struct ThreadgroupMem
		{
			uint32_t index;
			uint32_t size;
			uint32_t alignment;
		};

		std::optional<std::vector<ThreadgroupMem>> threadgroupMemory;
	};


	export class ComputeShader final : public Shader
	{
	protected:
		struct Kernel
		{
			NS::SharedPtr<MTL::Function> function;
			NS::SharedPtr<MTL::ComputePipelineState> pipeline;
			std::map<std::string, NS::UInteger> bufferBindings;
			///< (offset, index)
		};


		struct Buffer
		{
			NS::SharedPtr<MTL::Buffer> buffer;
			NS::UInteger size{};
		};

		std::map<std::string, Kernel> m_kernels;
		std::map<std::string, Buffer> m_boundBuffers;
		std::string m_path;

	public:
		explicit ComputeShader(const ComputeShaderCreateInfo& createInfo);

		ComputeShader& dispatch(const std::string& kernelName, size_t threadCount);

		KernelReflectionInfo kernelReflection(std::string_view name);

		ComputeShader& setBuffer(const std::string_view name,
		                         const NS::SharedPtr<MTL::Buffer>& buffer)
		{
			m_boundBuffers[std::string(name)] = {buffer, buffer->length()};
			return *this;
		}

		template <typename T>
		ComputeShader& setBuffer(const std::string_view name,
		                         const std::vector<T>& buffer)
		{
			// Allocate Metal buffer
			NS::SharedPtr<MTL::Buffer> mtlBuffer = NS::TransferPtr(
				m_device->newBuffer(buffer.size() * sizeof(T), MTL::ResourceStorageModeShared));

			// Copy data into GPU buffer
			std::memcpy(mtlBuffer->contents(), buffer.data(), buffer.size() * sizeof(T));

			// Store buffer with size in bytes
			m_boundBuffers[std::string(name)] = {
				mtlBuffer,
				static_cast<NS::UInteger>(buffer.size() * sizeof(T))
			};

			return *this;
		}

		template <typename T>
		ComputeShader& setBuffer(const std::string_view name,
		                         const std::span<T>& buffer)
		{
			// Allocate Metal buffer
			NS::SharedPtr<MTL::Buffer> mtlBuffer = NS::TransferPtr(
				m_device->newBuffer(buffer.size() * sizeof(T), MTL::ResourceStorageModeShared));

			// Copy data into GPU buffer
			std::memcpy(mtlBuffer->contents(), buffer.data(), buffer.size() * sizeof(T));

			// Store buffer with size in bytes
			m_boundBuffers[std::string(name)] = {
				mtlBuffer,
				static_cast<NS::UInteger>(buffer.size() * sizeof(T))
			};

			return *this;
		}

		[[nodiscard]] bool hasKernel(const std::string_view kernelName) const
		{
			return m_kernels.contains(std::string(kernelName));
		}

		[[nodiscard]] std::vector<std::string> kernelNames() const;

	private:
		void createPipelines();

		static KernelReflectionInfo createKernelReflectionInfo(std::string_view name,
		                                                       const MTL::ComputePipelineReflection*
		                                                       reflection);
	};
}
