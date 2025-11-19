module;
#include <iostream>
#include <string>
#include <Metal/Metal.hpp>
#include <map>
#include <span>
export module lune:metal_compute;

import :metal_shader;
import :metal_datatype_utils;

export namespace lune::metal
{
	struct KernelReflectionInfo
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


	class ComputeKernel
	{
		NS::SharedPtr<MTL::CommandBuffer> m_lastCommandBuffer;
		NS::SharedPtr<MTL::ComputePipelineState> m_pipeline;
		NS::SharedPtr<MTL::Function> m_function;
		MTL::Device* m_device;

		std::map<std::string, NS::UInteger> m_bindings;
		std::map<std::string, NS::SharedPtr<MTL::Buffer>> m_buffers;

		KernelReflectionInfo m_reflection{};

		std::string m_name;

	public:
		explicit ComputeKernel(MTL::Device* device, const std::string& name);

		[[nodiscard]] const std::string& name() const noexcept
		{
			return m_name;
		}

		ComputeKernel& dispatch(size_t threadCount, bool async = true);
		ComputeKernel& dispatch(size_t x, size_t y, size_t z, bool async = true);
		ComputeKernel& dispatch(size_t x, size_t y, size_t z,
		                        std::function<void()> callback, bool async = true);

		ComputeKernel& setBuffer(const std::string& name, const NS::SharedPtr<MTL::Buffer>& buf);
		ComputeKernel& setBytes(const std::string& name, const void* data, size_t size);

		template <typename T>
		ComputeKernel& setBuffer(const std::string& name, const std::vector<T>& vec)
		{
			const auto device = m_pipeline->device();
			const size_t byteSize = vec.size() * sizeof(T);

			NS::SharedPtr<MTL::Buffer> mtlBuffer =
				NS::TransferPtr(device->newBuffer(byteSize, MTL::ResourceStorageModeShared));

			std::memcpy(mtlBuffer->contents(), vec.data(), byteSize);

			m_buffers[name] = mtlBuffer;
			return *this;
		}

		MTL::Buffer* buffer(const std::string& name)
		{
			return m_buffers[name].get();
		}

		[[nodiscard]] KernelReflectionInfo reflection() const
		{
			return m_reflection;
		}

		void createPipeline(MTL::Library* library);
		ComputeKernel& waitUntilComplete();

	private:
		static KernelReflectionInfo createKernelReflectionInfo(const std::string& name,
		                                                       const MTL::ComputePipelineReflection*
		                                                       reflection);
	};


	class ComputeShader final : public Shader
	{
		std::map<std::string, std::unique_ptr<ComputeKernel>> m_kernels;
		std::string m_path;

	public:
		explicit ComputeShader(const std::string& path, MTL::Device* device = nullptr);

		ComputeKernel& kernel(const std::string& name);

		[[nodiscard]] std::vector<std::string> listKernels() const;

		[[nodiscard]] bool hasKernel(const std::string& name) const
		{
			return m_kernels.contains(name);
		}

	private:
		void createPipelines();
	};


	void printKernelInfo(const KernelReflectionInfo& info);
}
