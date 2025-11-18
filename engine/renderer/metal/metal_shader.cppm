module;
#include <iostream>
#include <string>
#include <string_view>
#include <Metal/Metal.hpp>
#include <map>
export module lune:metal_shader;

namespace lune::metal
{
	export struct GraphicsShaderCreateInfo
	{
		MTL::Device* device = nullptr;
		MTL::PixelFormat colorPixelFormat = MTL::PixelFormatBGRA8Unorm;
		std::string path;
		std::string vertexMain = "vertexMain";
		std::string fragmentMain = "fragmentMain";
		bool enableBlending = false;
	};


	export struct ComputeShaderCreateInfo
	{
		MTL::Device* device = nullptr;
		MTL::PipelineOption pipelineOption = MTL::PipelineOptionNone;
		MTL::AutoreleasedComputePipelineReflection* reflection = nullptr;
		std::string path;
		std::vector<std::string> kernels;
	};


	export class Shader
	{
	protected:
		MTL::Device* m_device{};

	public:
		explicit Shader(MTL::Device* device);
		virtual ~Shader() = default;

		[[nodiscard]] NS::SharedPtr<MTL::Library> createLibrary(const std::string& path) const;

		[[nodiscard]] MTL::Device* device() const
		{
			return m_device;
		}
	};


	export class GraphicsShader : public Shader
	{
	protected:
		NS::SharedPtr<MTL::Buffer> m_vertexBuffer{};
		NS::SharedPtr<MTL::Function> m_vertexFunction;
		NS::SharedPtr<MTL::Function> m_fragmentFunction;
		NS::SharedPtr<MTL::RenderPipelineState> m_pipelineState;
		MTL::PixelFormat m_colorPixelFormat;
		std::string m_path;
		std::string m_vertexMain;
		std::string m_fragmentMain;

	public:
		explicit GraphicsShader(const GraphicsShaderCreateInfo& createInfo);

		virtual void encodeRenderCommand(MTL::RenderCommandEncoder* commandEncoder) = 0;

		virtual void setupPipelineDescriptor(MTL::RenderPipelineDescriptor* pipelineDescriptor)
		{
			// Do nothing by default
		}

		void loadFunctions();
		void createRenderPipeline();

		[[nodiscard]] MTL::Buffer* vertexBuffer() const
		{
			return m_vertexBuffer.get();
		}

		[[nodiscard]] MTL::RenderPipelineState* pipelineState() const
		{
			return m_pipelineState.get();
		}

		[[nodiscard]] MTL::Function* vertexFunction() const
		{
			return m_vertexFunction.get();
		}

		[[nodiscard]] MTL::Function* fragmentFunction() const
		{
			return m_fragmentFunction.get();
		}
	};


	export class ComputeShader : public Shader
	{
	protected:
		struct Kernel
		{
			NS::SharedPtr<MTL::Function> function;
			NS::SharedPtr<MTL::ComputePipelineState> pipeline;
			NS::UInteger maxThreads;
			std::map<std::string, std::pair<NS::UInteger, NS::UInteger>> bufferBindings;
			///< (offset, index)
		};


		struct Buffer
		{
			NS::SharedPtr<MTL::Buffer> buffer;
		};

		std::map<std::string, Kernel> m_kernels;
		std::map<std::string, Buffer> m_boundBuffers;
		std::vector<NS::SharedPtr<MTL::ComputePipelineState>> m_pipelines;
		std::string m_path;

	public:
		explicit ComputeShader(const ComputeShaderCreateInfo& createInfo);

		void createPipelines();

		void dispatch(const std::string& kernelName, size_t threadCount);

		void setBuffer(const std::string_view name, const NS::SharedPtr<MTL::Buffer>& buffer)
		{
			m_boundBuffers[std::string(name)] = {buffer};
		}

		[[nodiscard]] bool hasKernel(std::string_view kernelName) const
		{
			return m_kernels.contains(std::string(kernelName));
		}
	};
}
