module;
#include <iostream>
#include <string>
#include <Metal/Metal.hpp>
#include <map>
#include <QuartzCore/QuartzCore.hpp>
export module lune:metal_shader;

import :graphics_types;

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

		NS::SharedPtr<MTL::RenderPipelineState> m_state;
		GraphicsPipelineDesc m_desc;

		std::vector<ArgumentInfo> m_vertexArguments;
		std::vector<ArgumentInfo> m_fragmentArguments;;

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

		[[nodiscard]] const GraphicsPipelineDesc& desc() const
		{
			return m_desc;
		}

		[[nodiscard]] const std::vector<ArgumentInfo>& vertexArguments() const
		{
			return m_vertexArguments;
		}

		[[nodiscard]] const std::vector<ArgumentInfo>& fragmentArguments() const
		{
			return m_fragmentArguments;
		}

	private:
		void createPipeline();
		static std::vector<ArgumentInfo> parse(const NS::Array* arguments);
	};


	export class Material
	{
		const GraphicsPipeline* m_pipeline{};

		std::map<std::string, NS::SharedPtr<MTL::Buffer>> m_uniformBuffers;
		std::map<std::string, NS::SharedPtr<MTL::Texture>> m_textures;

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

		Material& setUniform(const std::string& name, const void* data, size_t size,
		                     BufferUsage usage = Shared);
		Material& setUniform(const std::string& name, MTL::Texture* texture);
		Material& setUniform(const std::string& name, MTL::Buffer* buffer);

		void bind(MTL::RenderCommandEncoder* encoder) const;

		[[nodiscard]] const GraphicsPipeline& pipeline() const
		{
			return *m_pipeline;
		}
	};


	export class RenderPass
	{
		MTL::RenderCommandEncoder* m_encoder{};
		const GraphicsPipeline* m_pipeline{};
		NS::SharedPtr<MTL::CommandBuffer> m_commandBuffer{};

	public:
		[[nodiscard]] MTL::CommandBuffer* commandBuffer() const
		{
			return m_commandBuffer.get();
		}

		[[nodiscard]] MTL::RenderCommandEncoder* encoder() const
		{
			return m_encoder;
		}

		RenderPass& bind(const GraphicsPipeline& pipeline);
		RenderPass& bind(const Material& material);

		void begin(const CA::MetalDrawable* drawable);
		void end(const CA::MetalDrawable* drawable) const;

		RenderPass& draw(PrimitiveType type, uint startVertex, uint vertexCount);
		RenderPass& setFillMode(FillMode mode);
		RenderPass& waitUntilComplete();
	};
}
