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
		std::string path;
		std::string computeMain = "computeMain";
		MTL::PipelineOption pipelineOption = MTL::PipelineOptionNone;
		MTL::AutoreleasedComputePipelineReflection* reflection = nullptr;
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

		virtual void encodeRenderCommand(MTL::RenderCommandEncoder* renderCommandEncoder) = 0;

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
		NS::SharedPtr<MTL::ComputePipelineState> m_computePipelineState;
		NS::SharedPtr<MTL::Function> m_computeFunction;
		MTL::PipelineOption m_pipelineOption;
		MTL::ComputePipelineReflection* m_pipelineReflection;
		std::string m_path;
		std::string m_computeMain;

	public:
		explicit ComputeShader(const ComputeShaderCreateInfo& createInfo);

		virtual void encodeComputeCommand(MTL::ComputeCommandEncoder* encoder) = 0;

		virtual void setupPipelineDescriptor(MTL::ComputePipelineDescriptor* pipelineDescriptor)
		{
		}

		void loadFunction();
		void createComputePipeline();

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

		[[nodiscard]] MTL::ComputePipelineState* pipelineState() const
		{
			return m_computePipelineState.get();
		}

		[[nodiscard]] MTL::Function* computeFunction() const
		{
			return m_computeFunction.get();
		}
	};
}
