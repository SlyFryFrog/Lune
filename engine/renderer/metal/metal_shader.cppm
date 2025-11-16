module;
#include <string>
#include <Metal/Metal.hpp>
export module lune:metal_shader;


namespace lune::metal
{
	export class Shader
	{
	protected:
		NS::SharedPtr<MTL::Device> m_device{};

	public:
		explicit Shader(const NS::SharedPtr<MTL::Device>& device);
		virtual ~Shader() = default;

		virtual void encodeRenderCommand(MTL::RenderCommandEncoder* renderCommandEncoder) = 0;

		NS::SharedPtr<MTL::Device> device()
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
		std::string m_path;
		std::string m_vertexMain;
		std::string m_fragmentMain;

	public:
		explicit GraphicsShader(const NS::SharedPtr<MTL::Device>& device, const std::string& path,
		                        const std::string& vertexMain = "vertexMain",
		                        const std::string& fragmentMain = "fragmentMain");

		void createLibrary();
		void createRenderPipeline();

		[[nodiscard]] MTL::Buffer* vertexBuffer() const
		{
			return m_vertexBuffer.get();
		}

		[[nodiscard]] MTL::RenderPipelineState* getPipelineState() const
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
}
