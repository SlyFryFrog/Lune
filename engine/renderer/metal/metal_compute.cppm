module;
#include <iostream>
#include <string>
#include <Metal/Metal.hpp>
#include <map>
export module lune:metal_compute;

import :metal_shader;
import :metal_datatype_utils;

namespace lune::metal
{
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


	export class ComputeKernel
	{
		NS::SharedPtr<MTL::CommandBuffer> m_lastCommandBuffer;
		NS::SharedPtr<MTL::ComputePipelineState> m_pipeline;
		NS::SharedPtr<MTL::Function> m_function;
		MTL::Device* m_device;

		std::map<std::string, NS::UInteger> m_bindings;
		std::map<std::string, NS::UInteger> m_textureBindings;
		std::map<std::string, MTL::Buffer*> m_buffers;
		std::map<std::string, MTL::Texture*> m_textures;

		KernelReflectionInfo m_reflection{};

		std::string m_name;

	public:
		explicit ComputeKernel(MTL::Device* device, const std::string& name);

		[[nodiscard]] const std::string& name() const noexcept
		{
			return m_name;
		}

		[[nodiscard]] std::map<std::string, MTL::Buffer*>& buffers() noexcept
		{
			return m_buffers;
		}

		ComputeKernel& dispatch(size_t threadCount);
		ComputeKernel& dispatch(size_t x, size_t y, size_t z);
		ComputeKernel& dispatch(size_t x, size_t y, size_t z, std::function<void()> callback);
		ComputeKernel& dispatch(const MTL::Size& threadGroups, const MTL::Size& threadsPerGroup);

		template <typename T>
		ComputeKernel& setUniform(const std::string& name, T& data,
		                       BufferUsage options = BufferUsage::Shared);
		ComputeKernel& setUniform(const std::string& name, const void* data, size_t size,
		                        BufferUsage options = BufferUsage::Shared);

		ComputeKernel& setUniform(const std::string& name, MTL::Buffer* buffer);

		ComputeKernel& setUniform(const std::string& name, MTL::Texture* texture);

		MTL::Texture* texture(const std::string& name);

		MTL::Buffer* buffer(const std::string& name)
		{
			return m_buffers[name];
		}

		[[nodiscard]] KernelReflectionInfo reflection() const
		{
			return m_reflection;
		}

		void createPipeline(MTL::Library* library);
		ComputeKernel& waitUntilComplete();

		static void bufferToTexture(const MTL::Buffer* buffer, const MTL::Texture* texture,
		                            NS::UInteger bytesPerRow, const MTL::Size& sourceSize,
		                            bool waitUntilComplete = true);

	private:
		static KernelReflectionInfo createKernelReflectionInfo(const std::string& name,
		                                                       const MTL::ComputePipelineReflection*
		                                                       reflection);

		void bindBuffers(MTL::ComputeCommandEncoder* commandEncoder);
		void bindTextures(MTL::ComputeCommandEncoder* commandEncoder);
	};


	template <typename T>
	ComputeKernel& ComputeKernel::setUniform(const std::string& name, T& data,
	                                      const BufferUsage options)
	{
		// Allocate a small temp buffer for byte data
		const auto device = m_pipeline->device();
		auto temp = device->newBuffer(sizeof(T), static_cast<MTL::ResourceOptions>(options));

		std::memcpy(temp->contents(), &data, sizeof(T));
		m_buffers[name] = temp;

		return *this;
	}


	export class ComputeShader final : public Shader
	{
		std::map<std::string, std::unique_ptr<ComputeKernel>> m_kernels;
		std::string m_path;

	public:
		/**
		 * @brief Initializes the shader and creates pipelines for all kernels declared in the shader.
		 *
		 * @param path Path to the compute shader. (.metal, .metallib, .slang)
		 * @param device The device (GPU) to use for the shader. When not set, defaults to the current device set in MetalContext.
		 */
		explicit ComputeShader(const std::string& path, MTL::Device* device = nullptr);

		/**
		 * @brief Returns the ComputeKernel with the given name.
		 *
		 * @param name Name of the kernel.
		 * @return Reference to the ComputeKernel instance.
		 */
		ComputeKernel& kernel(const std::string& name);

		/**
		 * @brief Returns a list of all kernel names.
		 *
		 * @return Names of all the kernels loaded for the ComputeShader.
		 */
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
