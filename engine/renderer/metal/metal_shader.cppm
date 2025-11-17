module;
#include <string>
#include <Metal/Metal.hpp>
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
		std::vector<std::string> kernels;
		std::string path;
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
		MTL::ComputePipelineReflection* m_pipelineReflection{};
		MTL::PipelineOption m_pipelineOption;
		std::vector<NS::SharedPtr<MTL::ComputePipelineState>> m_pipelines;
		std::vector<std::string> m_kernelNames;
		std::string m_path;

	public:
		explicit ComputeShader(const ComputeShaderCreateInfo& createInfo);

		virtual void encodeComputeCommand(MTL::ComputeCommandEncoder* commandEncoder) = 0;

		virtual void setupPipelineDescriptor(MTL::ComputePipelineDescriptor* pipelineDescriptor)
		{
		}

		void createPipelines();

		void addKernel(const std::string& name)
		{
			m_kernelNames.push_back(name);
		}

		void setPipelineReflection(MTL::ComputePipelineReflection* pipelineReflection)
		{
			m_pipelineReflection = pipelineReflection;
		}

		[[nodiscard]] MTL::AutoreleasedComputePipelineReflection* pipelineReflection()
		{
			return &m_pipelineReflection;
		}

		void setPipelineOption(const MTL::PipelineOption pipelineOption)
		{
			m_pipelineOption = pipelineOption;
		}

		[[nodiscard]] MTL::PipelineOption pipelineOption() const
		{
			return m_pipelineOption;
		}

		[[nodiscard]] MTL::ComputePipelineState* pipelineState(const size_t i) const
		{
			return m_pipelines[i].get();
		}

		[[nodiscard]] size_t pipelineCount() const
		{
			return m_pipelines.size();
		}
	};
}
