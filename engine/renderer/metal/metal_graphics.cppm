module;
#include <Metal/Metal.hpp>
#include <iostream>
#include <map>
export module lune.metal:graphics;

import lune.gfx;

namespace lune::metal
{
	MTL::Library* createLibrary(const std::string& path, MTL::Device* device, NS::Error** error);

	export class MetalShaderImpl : public gfx::IShaderImpl
	{
		MTL::Device* m_device{};

		NS::SharedPtr<MTL::Library> m_library;
		NS::SharedPtr<MTL::Function> m_vertex{};
		NS::SharedPtr<MTL::Function> m_fragment{};

		gfx::ShaderDesc m_desc{};

	public:
		explicit MetalShaderImpl(MTL::Device* device, gfx::ShaderDesc desc) :
			m_device(device), m_desc(desc)
		{
			MetalShaderImpl::create();
		}

		[[nodiscard]] MTL::Device* device() const noexcept
		{
			return m_device;
		}

		[[nodiscard]] MTL::Function* vertex() const noexcept
		{
			return m_vertex.get();
		}

		[[nodiscard]] MTL::Function* fragment() const noexcept
		{
			return m_fragment.get();
		}

		void create() override;
	};


	export class MetalMaterialImpl : public gfx::IMaterialImpl
	{
		const gfx::Pipeline* m_pipeline{};

		std::map<std::string, MTL::Buffer*> m_uniformBuffers;
		std::map<std::string, MTL::Texture*> m_textures;

	public:
		explicit MetalMaterialImpl(const gfx::Pipeline& pipeline) : m_pipeline(&pipeline)
		{
		}

		void setUniform(const std::string& name, const gfx::Texture& texture) override;
		void setUniform(const std::string& name, const gfx::Buffer& buffer) override;
		void setUniform(const std::string& name, const void* data, size_t size) override;

		void bind(MTL::RenderCommandEncoder* encoder) const;

		[[nodiscard]] const gfx::Pipeline& pipeline() const
		{
			return *m_pipeline;
		}
	};


	export class MetalPipelineImpl : public gfx::IPipelineImpl
	{
		struct ArgumentInfo
		{
			std::string name;
			uint32_t index;
			uint32_t arrayLength;
			MTL::ArgumentType type;
		};

		const gfx::Shader* m_shader{};
		gfx::PipelineDesc m_desc{};

		NS::SharedPtr<MTL::RenderPipelineState> m_state;
		NS::SharedPtr<MTL::DepthStencilState> m_depthStencilState;

		std::vector<ArgumentInfo> m_vertexArguments;
		std::vector<ArgumentInfo> m_fragmentArguments;

	public:
		explicit MetalPipelineImpl(const gfx::Shader& shader, const gfx::PipelineDesc& desc) :
			m_shader(&shader), m_desc(desc)
		{
			MetalPipelineImpl::createPipeline();
		}

		~MetalPipelineImpl() override = default;

		[[nodiscard]] const std::vector<ArgumentInfo>& vertexArguments() const noexcept
		{
			return m_vertexArguments;
		}

		[[nodiscard]] const std::vector<ArgumentInfo>& fragmentArguments() const noexcept
		{
			return m_fragmentArguments;
		}

		[[nodiscard]] MTL::RenderPipelineState* state() const noexcept
		{
			return m_state.get();
		}

		[[nodiscard]] MTL::DepthStencilState* depthStencilState() const noexcept
		{
			return m_depthStencilState.get();
		}

	private:
		void createPipeline() override;
		static std::vector<ArgumentInfo> parse(const NS::Array* arguments);
	};


	export class MetalRenderPassImpl : public gfx::IRenderPassImpl
	{
		NS::SharedPtr<MTL::RenderCommandEncoder> m_encoder{};
		NS::SharedPtr<MTL::CommandBuffer> m_commandBuffer{};
		const gfx::RenderSurface& m_surface;

	public:
		explicit MetalRenderPassImpl(const gfx::RenderSurface& surface) : m_surface(surface)
		{
		}

		~MetalRenderPassImpl() override = default;

		void bind(const gfx::IMaterialImpl& material) override;

		void begin() override;
		void end() override;
		void draw(gfx::PrimitiveType type, std::uint32_t start, uint32_t count) override;
		void drawIndexed(gfx::PrimitiveType type, uint32_t indexCount,
						 const gfx::Buffer& indexBuffer, uint32_t indexOffset) override;

		void setViewport(float x, float y, float w, float h, float zmin, float zmax) override;
		void setScissor(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;

		void waitUntilComplete() override;

		void setFillMode(gfx::FillMode fillMode) override;
	};


	constexpr MetalShaderImpl* toMetalImpl(const gfx::Shader& shader)
	{
		const auto impl = shader.getImpl();

#ifndef NDEBUG
		const auto metalImpl = dynamic_cast<MetalShaderImpl*>(impl);
		if (!metalImpl)
			throw std::runtime_error("Shader is not a Metal shader!");
		return metalImpl;
#else
		return static_cast<MetalShaderImpl*>(impl);
#endif
	}


	constexpr MetalMaterialImpl* toMetalImpl(const gfx::Material& material)
	{
		const auto impl = material.getImpl();

#ifndef NDEBUG
		const auto metalImpl = dynamic_cast<MetalMaterialImpl*>(impl);
		if (!metalImpl)
			throw std::runtime_error("Material is not a Metal material!");
		return metalImpl;
#else
		return static_cast<MetalMaterialImpl*>(impl);
#endif
	}


	constexpr MetalPipelineImpl* toMetalImpl(const gfx::Pipeline& pipeline)
	{
		const auto impl = pipeline.getImpl();

#ifndef NDEBUG
		const auto metalImpl = dynamic_cast<MetalPipelineImpl*>(impl);
		if (!metalImpl)
			throw std::runtime_error("Pipeline is not a Metal render pipeline!");
		return metalImpl;
#else
		return static_cast<lune::metal::MetalPipelineImpl*>(impl);
#endif
	}


	constexpr MetalRenderPassImpl* toMetalImpl(const gfx::RenderPass& renderPass)
	{
		const auto impl = renderPass.getImpl();

#ifndef NDEBUG
		const auto metalImpl = dynamic_cast<MetalRenderPassImpl*>(impl);
		if (!metalImpl)
			throw std::runtime_error("RenderPass is not a Metal render pass!");
		return metalImpl;
#else
		return static_cast<MetalRenderPassImpl*>(impl);
#endif
	}
} // namespace lune::metal
