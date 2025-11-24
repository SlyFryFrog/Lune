module;
#include <iostream>
#include <string>
#include <Metal/Metal.hpp>
#include <map>
#include <QuartzCore/QuartzCore.hpp>
export module lune:metal_shader;

namespace lune::metal
{
	export struct GraphicsReflectionInfo
	{
		struct Resource
		{
			std::string name;

			NS::UInteger index;
			MTL::ArgumentType type;
			MTL::DataType dataType;

			bool isVertexStage;
			bool isFragmentStage;
			size_t dataSize;
		};

		std::vector<Resource> resources;
	};


	export struct GraphicsPipelineDesc
	{
		MTL::PixelFormat colorFormat = MTL::PixelFormatBGRA8Unorm;
		MTL::PixelFormat depthFormat = MTL::PixelFormatDepth32Float;

		bool enableBlending = false;
		bool depthWrite = true;

		MTL::CompareFunction depthCompare = MTL::CompareFunctionLess;
		MTL::CullMode cullMode = MTL::CullModeBack;
		MTL::Winding winding = MTL::WindingCounterClockwise;
	};


	export class GraphicsShader
	{
		MTL::Device* m_device{};

		NS::SharedPtr<MTL::Library> m_library;

		NS::SharedPtr<MTL::Function> m_vertex;
		NS::SharedPtr<MTL::Function> m_fragment;

	public:
		explicit GraphicsShader(const std::string& path,
		                      const std::string& vsName = "vertexMain",
		                      const std::string& fsName = "fragmentMain",
		                      MTL::Device* device = nullptr);

		[[nodiscard]] MTL::Device* device() const
		{
			return m_device;
		}

		[[nodiscard]] MTL::Function* vertex() const
		{
			return m_vertex.get();
		}

		[[nodiscard]] MTL::Function* fragment() const
		{
			return m_fragment.get();
		}

	private:
		void createLibrary(const std::string& path, NS::Error** error);
	};


	export class GraphicsPipeline
	{
		const GraphicsShader* m_shader{};

		NS::SharedPtr<MTL::RenderPipelineState> m_state;
		GraphicsPipelineDesc m_desc;
		GraphicsReflectionInfo m_reflection;

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

		[[nodiscard]] const GraphicsReflectionInfo& reflection() const
		{
			return m_reflection;
		}

	private:
		void createPipeline();

		GraphicsReflectionInfo buildReflection(const MTL::RenderPipelineReflection* reflection);
	};


	export class Material
	{
		const GraphicsPipeline* m_pipeline{};

		std::map<std::string, NS::SharedPtr<MTL::Buffer>> m_uniformBuffers;
		std::map<std::string, MTL::Texture*> m_textures;

	public:
		explicit Material(const GraphicsPipeline& pipeline) :
			m_pipeline(&pipeline)
		{
		}

		template <typename T>
		void setUniform(const std::string& name, const T& value)
		{
			setUniform(name, &value, sizeof(T));
		}

		void setUniform(const std::string& name, const void* data, size_t size);
		void setUniform(const std::string& name, MTL::Texture* texture);

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
		explicit RenderPass(const GraphicsPipeline* pipeline) :
			m_pipeline(pipeline)
		{
			m_encoder->setRenderPipelineState(m_pipeline->state());
		}

		void bind(const GraphicsPipeline& pipeline) const
		{
			m_encoder->setRenderPipelineState(pipeline.state());
		}

		void bind(const Material& material) const
		{
			material.bind(m_encoder);
		}

		void setVertexBuffer(const MTL::Buffer* buffer, const uint offset, const uint index) const
		{
			m_encoder->setVertexBuffer(buffer, offset, index);
		}

		void setFragmentBuffer(const MTL::Buffer* buffer, const uint offset, const uint index) const
		{
			m_encoder->setFragmentBuffer(buffer, offset, index);
		}

		void setFragmentTexture(const MTL::Texture* texture, const uint index) const
		{
			m_encoder->setFragmentTexture(texture, index);
		}

		void begin(const CA::MetalDrawable* drawable);

		void end(const CA::MetalDrawable* drawable) const;

		void draw(MTL::PrimitiveType type, uint vertexCount, uint startVertex) const;
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
}
