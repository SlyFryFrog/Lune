module;
#include <string>
#include <Metal/Metal.hpp>
export module lune:metal_shader;


namespace lune::metal
{
	export class Shader
	{
	protected:
		NS::SharedPtr<MTL::Buffer> m_buffer{};

	public:
		Shader() = default;
		virtual ~Shader() = default;

		virtual void encodeRenderCommand(MTL::RenderCommandEncoder* renderCommandEncoder) = 0;

		[[nodiscard]] MTL::Buffer* buffer() const
		{
			return m_buffer.get();
		}
	};


	export class GraphicsShader : public Shader
	{
	protected:
		NS::SharedPtr<MTL::Function> m_vertexFunction;
		NS::SharedPtr<MTL::Function> m_fragmentFunction;
		NS::SharedPtr<MTL::RenderPipelineState> m_pipelineState;
		std::string m_path;
		std::string m_vertexMain;
		std::string m_fragmentMain;

	public:
		explicit GraphicsShader(
			std::string  path,
			std::string  vertexMain = "vertexMain",
			std::string  fragmentMain = "fragmentMain"
			);

		void createLibrary();
		void createRenderPipeline();

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


	export class ShaderProgram
	{
	public:
		ShaderProgram() = default;
		~ShaderProgram() = default;
	};
}
