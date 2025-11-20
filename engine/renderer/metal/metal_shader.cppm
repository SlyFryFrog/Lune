module;
#include <iostream>
#include <string>
#include <Metal/Metal.hpp>
#include <map>
export module lune:metal_shader;

namespace lune::metal
{
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


	export struct GraphicsShaderCreateInfo
	{
		MTL::Device* device = nullptr;
		MTL::PixelFormat colorPixelFormat = MTL::PixelFormatBGRA8Unorm;
		std::string path;
		std::string vertexMain = "vertexMain";
		std::string fragmentMain = "fragmentMain";
		bool enableBlending = false;
	};


	export class GraphicsShader : public Shader
	{
	protected:
		NS::SharedPtr<MTL::Buffer> m_vertexBuffer{};
		NS::SharedPtr<MTL::Function> m_vertexFunction;
		NS::SharedPtr<MTL::Function> m_fragmentFunction;
		NS::SharedPtr<MTL::RenderPipelineState> m_pipelineState;
		MTL::Texture* m_texture;
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
}
