module;
#include <iostream>
#include <string>
#include <Metal/Metal.hpp>
#include <map>
#include <span>
export module lune:metal_compute;

import :metal_shader;

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


	struct ComputeShaderCreateInfo
	{
		MTL::Device* device;
		std::string path;
		std::vector<std::string> kernels;
	};


	class ComputeKernel
	{
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

		ComputeKernel& dispatch(size_t threadCount);
		ComputeKernel& dispatch(size_t x, size_t y, size_t z = 1);

		ComputeKernel& setBuffer(const std::string& name, const NS::SharedPtr<MTL::Buffer>& buf)
		{
			m_buffers[name] = buf;
			return *this;
		}

		ComputeKernel& setBytes(const std::string& name, const void* data, const size_t size)
		{
			// Allocate a small temp buffer for byte data
			const auto device = m_pipeline->device();
			NS::SharedPtr<MTL::Buffer> temp =
				NS::TransferPtr(device->newBuffer(size, MTL::ResourceStorageModeShared));

			std::memcpy(temp->contents(), data, size);
			m_buffers[name] = temp;

			return *this;
		}

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
		explicit ComputeShader(const ComputeShaderCreateInfo& info);

		[[nodiscard]] bool hasKernel(const std::string& name) const
		{
			return m_kernels.contains(name);
		}

		ComputeKernel& kernel(const std::string& name)
		{
			if (!m_kernels.contains(name))
			{
				throw std::runtime_error("Kernel name not found");
			}

			return *m_kernels[name];
		}

		[[nodiscard]] std::vector<std::string> listKernels() const
		{
			std::vector<std::string> names;
			for (const auto& [name, kernel] : m_kernels)
			{
				names.push_back(name);
			}

			return names;
		}

	private:
		void createPipelines();
	};


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
					<< "    Type      : " << arg.type << "\n"
					<< "    Data Type : " << arg.dataType << "\n"
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
