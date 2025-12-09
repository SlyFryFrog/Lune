module;
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <iostream>
#include <map>
#include <string>
export module lune:metal_shader;

import :graphics_types;
import :metal_context;

namespace lune::metal
{
	export struct GraphicsPipelineDesc
	{
		gfx::PixelFormat colorFormat = gfx::PixelFormat::RGBA8_UNorm;
		gfx::PixelFormat depthFormat = gfx::PixelFormat::Depth32_Float;

		bool enableBlending = false;
		bool depthWrite = true;

		MTL::CompareFunction depthCompare = MTL::CompareFunctionLess;
		gfx::CullMode cullMode = gfx::CullMode::Back;
		gfx::Winding winding = gfx::Winding::CounterClockwise;
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

		[[nodiscard]] MTL::Device* device() const noexcept
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
		explicit GraphicsShader(const std::string& path, const std::string& vsName = "vertexMain",
								const std::string& fsName = "fragmentMain",
								MTL::Device* device = nullptr);

		[[nodiscard]] MTL::Function* vertex() const noexcept
		{
			return m_vertex.get();
		}

		[[nodiscard]] MTL::Function* fragment() const noexcept
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

		[[nodiscard]] MTL::Device* device() const noexcept
		{
			return m_shader->device();
		}

		[[nodiscard]] MTL::RenderPipelineState* state() const noexcept
		{
			return m_state.get();
		}

		[[nodiscard]] const GraphicsShader& shader() const noexcept
		{
			return *m_shader;
		}

		[[nodiscard]] const std::vector<ArgumentInfo>& vertexArguments() const noexcept
		{
			return m_vertexArguments;
		}

		[[nodiscard]] const std::vector<ArgumentInfo>& fragmentArguments() const noexcept
		{
			return m_fragmentArguments;
		}

		[[nodiscard]] MTL::DepthStencilState* depthStencilState() const noexcept
		{
			return m_depthStencilState.get();
		}

		[[nodiscard]] gfx::CullMode cullMode() const noexcept
		{
			return m_desc.cullMode;
		}

		[[nodiscard]] gfx::Winding winding() const noexcept
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
		explicit Material(const GraphicsPipeline& pipeline) : m_pipeline(&pipeline)
		{
		}

		template <typename T> Material& setUniform(const std::string& name, const T& value)
		{
			setUniform(name, &value, sizeof(T));
			return *this;
		}

		Material& setUniform(const std::string& name, const gfx::Texture& texture);
		Material& setUniform(const std::string& name, const gfx::Buffer& buffer);

		[[nodiscard]] bool hasUniform(const std::string& name) const
		{
			return m_uniformBuffers.contains(name) || m_textures.contains(name);
		}

		[[nodiscard]] bool hasTexture(const std::string& name) const
		{
			return m_textures.contains(name);
		}

		[[nodiscard]] bool hasBuffer(const std::string& name) const
		{
			return m_uniformBuffers.contains(name);
		}

		Material& clear()
		{
			m_uniformBuffers.clear();
			m_textures.clear();
			return *this;
		}

		Material& remove(const std::string& name)
		{
			m_textures.erase(name);
			m_uniformBuffers.erase(name);
			return *this;
		}

		Material& setPipeline(const GraphicsPipeline& pipeline)
		{
			m_pipeline = &pipeline;
			return *this;
		}

		Material& setUniform(const std::string& name, const void* data, size_t size,
							 gfx::BufferUsage usage = gfx::Shared);

		void bind(MTL::RenderCommandEncoder* encoder) const;

		[[nodiscard]] const GraphicsPipeline& pipeline() const
		{
			return *m_pipeline;
		}
	};


	export class RenderPass
	{
		NS::SharedPtr<MTL::RenderCommandEncoder> m_encoder{};
		NS::SharedPtr<MTL::CommandBuffer> m_commandBuffer{};
		RenderSurface& m_surface;

	public:
		explicit RenderPass(RenderSurface& surface) : m_surface(surface)
		{
		}

		RenderPass& bind(const Material& material);

		RenderPass& begin();
		RenderPass& end();

		RenderPass& draw(gfx::PrimitiveType type, uint startVertex, uint vertexCount);
		RenderPass& drawIndexed(gfx::PrimitiveType type, uint indexCount,
								const gfx::Buffer& indexBuffer, uint indexOffset = 0);

		RenderPass& setFillMode(gfx::FillMode mode);
		RenderPass& setCullMode(gfx::CullMode mode);
		RenderPass& waitUntilComplete();

		RenderPass& setViewport(float x, float y, float w, float h, float zmin = 0.0f,
								float zmax = 1.0f);
		RenderPass& setScissor(uint x, uint y, uint w, uint h);
	};
} // namespace lune::metal
