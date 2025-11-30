module;
#include <iostream>
#include <map>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <string>
export module lune:metal_shader;

import :graphics_types;
import :metal_context;

namespace lune::metal
{
	export struct GraphicsPipelineDesc
	{
		PixelFormat colorFormat = PixelFormat::RGBA8_UNorm;
		PixelFormat depthFormat = PixelFormat::Depth32_Float;

		bool enableBlending = false;
		bool depthWrite = true;

		MTL::CompareFunction depthCompare = MTL::CompareFunctionLess;
		MTL::CullMode cullMode = MTL::CullModeBack;
		MTL::Winding winding = MTL::WindingCounterClockwise;
	};


	export class Shader
	{
	protected:
		MTL::Device* m_device{};
		NS::SharedPtr<MTL::Library> m_library;

	public:
		explicit Shader(MTL::Device* device);
		virtual ~Shader() = default;

		void createLibrary(const std::string& path, NS::Error** error);

		[[nodiscard]] MTL::Device* device() const
		{
			return m_device;
		}
	};


	export class GraphicsShader final : public Shader
	{
		MTL::Device* m_device{};

		NS::SharedPtr<MTL::Function> m_vertex;
		NS::SharedPtr<MTL::Function> m_fragment;

		std::string m_path;
		std::string m_vertexMainName;
		std::string m_fragmentMainName;

	public:
		explicit GraphicsShader(const std::string& path,
		                        const std::string& vsName = "vertexMain",
		                        const std::string& fsName = "fragmentMain",
		                        MTL::Device* device = nullptr);

		[[nodiscard]] MTL::Function* vertex() const
		{
			return m_vertex.get();
		}

		[[nodiscard]] MTL::Function* fragment() const
		{
			return m_fragment.get();
		}

	private:
		void create();
	};


	export class GraphicsPipeline
	{
		struct ArgumentInfo
		{
			std::string name;
			uint32_t index;
			uint32_t arrayLength;
			MTL::ArgumentType type;
		};

		const GraphicsShader* m_shader{};
		GraphicsPipelineDesc m_desc;

		NS::SharedPtr<MTL::RenderPipelineState> m_state;
		NS::SharedPtr<MTL::DepthStencilState> m_depthStencilState;

		std::vector<ArgumentInfo> m_vertexArguments;
		std::vector<ArgumentInfo> m_fragmentArguments;

	public:
		explicit GraphicsPipeline(const GraphicsShader& shader,
		                          const GraphicsPipelineDesc& desc = GraphicsPipelineDesc());

		[[nodiscard]] MTL::Device* device() const
		{
			return m_shader->device();
		}

		[[nodiscard]] MTL::RenderPipelineState* state() const
		{
			return m_state.get();
		}

		[[nodiscard]] const GraphicsShader& shader() const
		{
			return *m_shader;
		}

		[[nodiscard]] const std::vector<ArgumentInfo>& vertexArguments() const
		{
			return m_vertexArguments;
		}

		[[nodiscard]] const std::vector<ArgumentInfo>& fragmentArguments() const
		{
			return m_fragmentArguments;
		}

		[[nodiscard]] MTL::DepthStencilState* depthStencilState() const
		{
			return m_depthStencilState.get();
		}

		[[nodiscard]] MTL::CullMode cullMode() const
		{
			return m_desc.cullMode;
		}

		[[nodiscard]] MTL::Winding winding() const
		{
			return m_desc.winding;
		}

	private:
		void createPipeline();
		static std::vector<ArgumentInfo> parse(const NS::Array* arguments);
	};


	export class Material
	{
		const GraphicsPipeline* m_pipeline{};

		std::map<std::string, MTL::Buffer*> m_uniformBuffers;
		std::map<std::string, MTL::Texture*> m_textures;

	public:
		explicit Material(const GraphicsPipeline& pipeline) :
			m_pipeline(&pipeline)
		{
		}

		template <typename T>
		Material& setUniform(const std::string& name, const T& value)
		{
			setUniform(name, &value, sizeof(T));
			return *this;
		}

		Material& setUniform(const std::string& name, const Texture& texture);
		Material& setUniform(const std::string& name, const Buffer& buffer);

		Material& setUniform(const std::string& name, const void* data, size_t size,
		                     BufferUsage usage = Shared);

		[[nodiscard]] const GraphicsPipeline& pipeline() const
		{
			return *m_pipeline;
		}

		void bind(MTL::RenderCommandEncoder* encoder) const;
	};


	export class RenderPass
	{
		NS::SharedPtr<MTL::RenderCommandEncoder> m_encoder{};
		NS::SharedPtr<MTL::CommandBuffer> m_commandBuffer{};
		const RenderSurface* m_surface = nullptr;

	public:
		RenderPass& bind(const Material& material);

		RenderPass& begin(RenderSurface& surface);
		RenderPass& end();

		RenderPass& draw(PrimitiveType type, uint startVertex, uint vertexCount);
		RenderPass& setFillMode(FillMode mode);
		RenderPass& waitUntilComplete();
	};
}
