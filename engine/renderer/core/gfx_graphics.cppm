module;
#include <memory>
#include <string>
#include <vector>
export module lune.gfx:graphics;

import :render_surface;
import :types;
import :buffer;
import :texture;

namespace lune::gfx
{
	export struct ShaderDesc
	{
		std::string path;
		std::string vsMain{"vertexMain"};
		std::string fsMain{"fragmentMain"};
	};

	export struct PipelineDesc
	{
		PixelFormat colorFormat{PixelFormat::RGBA8_UNorm};
		PixelFormat depthFormat{PixelFormat::Depth32_Float};
		bool enableBlending{false};

		CullMode cullMode = CullMode::Back;
		Winding winding = Winding::CounterClockwise;
	};


	export class IShaderImpl
	{
	public:
		explicit IShaderImpl() = default;
		virtual ~IShaderImpl() = default;

		virtual void create() = 0;
	};

	export class IPipelineImpl
	{
		PipelineDesc m_desc;

	public:
		explicit IPipelineImpl() = default;
		virtual ~IPipelineImpl() = default;

		[[nodiscard]] CullMode cullMode() const noexcept
		{
			return m_desc.cullMode;
		}

		[[nodiscard]] Winding winding() const noexcept
		{
			return m_desc.winding;
		}


	protected:
		virtual void createPipeline() = 0;
	};

	export class IMaterialImpl
	{
	public:
		explicit IMaterialImpl() = default;
		virtual ~IMaterialImpl() = default;

		virtual void setUniform(const std::string& name, const Texture& texture) = 0;
		virtual void setUniform(const std::string& name, const Buffer& buffer) = 0;

		virtual void setUniform(const std::string& name, const void* data, size_t size) = 0;
	};

	export class IRenderPassImpl
	{
	public:
		virtual ~IRenderPassImpl() = default;

		virtual void bind(const IMaterialImpl& material) = 0;

		virtual void begin() = 0;
		virtual void end() = 0;
		virtual void draw(PrimitiveType prim, uint32_t start, uint32_t count) = 0;
		virtual void drawIndexed(PrimitiveType type, uint32_t indexCount, const Buffer& indexBuffer,
								 uint32_t indexOffset) = 0;

		virtual void setViewport(float x, float y, float w, float h, float zmin, float zmax) = 0;
		virtual void setScissor(uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0;

		virtual void waitUntilComplete() = 0;

		virtual void setFillMode(FillMode fillMode) = 0;
	};

	export class Shader
	{
		std::unique_ptr<IShaderImpl> m_impl;

	public:
		explicit Shader(std::unique_ptr<IShaderImpl> impl) : m_impl(std::move(impl))
		{
		}

		[[nodiscard]] IShaderImpl* getImpl() const
		{
			return m_impl.get();
		}
	};

	export class Pipeline
	{
		std::unique_ptr<IPipelineImpl> m_impl;

	public:
		explicit Pipeline(std::unique_ptr<IPipelineImpl> impl) : m_impl(std::move(impl))
		{
		}

		virtual ~Pipeline() = default;

		[[nodiscard]] IPipelineImpl* getImpl() const
		{
			return m_impl.get();
		}

		[[nodiscard]] CullMode cullMode() const noexcept
		{
			return m_impl->cullMode();
		}

		[[nodiscard]] Winding winding() const noexcept
		{
			return m_impl->winding();
		}
	};

	export class Material
	{
		std::unique_ptr<IMaterialImpl> m_impl;

	public:
		explicit Material(std::unique_ptr<IMaterialImpl> impl) : m_impl(std::move(impl))
		{
		}

		[[nodiscard]] IMaterialImpl* getImpl() const
		{
			return m_impl.get();
		}

		Material& setUniform(const std::string& name, const Texture& texture)
		{
			m_impl->setUniform(name, texture);
			return *this;
		}

		Material& setUniform(const std::string& name, const Buffer& buffer)
		{
			m_impl->setUniform(name, buffer);
			return *this;
		}

		Material& setUniform(const std::string& name, const void* data, const size_t size)
		{
			m_impl->setUniform(name, data, size);
			return *this;
		}

		template <typename T> Material& setUniform(const std::string& name, const T& value)
		{
			setUniform(name, &value, sizeof(T));
			return *this;
		}
	};

	export class RenderPass
	{
		std::unique_ptr<IRenderPassImpl> m_impl;

	public:
		explicit RenderPass(std::unique_ptr<IRenderPassImpl> impl) : m_impl(std::move(impl))
		{
		}

		[[nodiscard]] IRenderPassImpl* getImpl() const
		{
			return m_impl.get();
		}

		RenderPass& bind(const Material& material)
		{
			const auto passImpl{getImpl()};
			const auto materialImpl{material.getImpl()};

			// Delegate binding to backend
			passImpl->bind(*materialImpl);

			return *this;
		}

		RenderPass& begin()
		{
			m_impl->begin();
			return *this;
		}

		RenderPass& end()
		{
			m_impl->end();
			return *this;
		}

		RenderPass& draw(const PrimitiveType type, const uint32_t start, const uint32_t count)
		{
			m_impl->draw(type, start, count);
			return *this;
		}

		RenderPass& drawIndexed(const PrimitiveType type, const uint32_t indexCount,
								const Buffer& indexBuffer, const uint32_t indexOffset)
		{
			m_impl->drawIndexed(type, indexCount, indexBuffer, indexOffset);
			return *this;
		}

		RenderPass& setViewport(const float x, const float y, const float w, const float h,
								const float zmin = 0.0f, const float zmax = 1.0f)
		{
			m_impl->setViewport(x, y, w, h, zmin, zmax);
			return *this;
		}

		RenderPass& setScissor(const uint32_t x, const uint32_t y, const uint32_t w,
							   const uint32_t h)
		{
			m_impl->setScissor(x, y, w, h);
			return *this;
		}

		RenderPass& waitUntilComplete()
		{
			m_impl->waitUntilComplete();
			return *this;
		}

		RenderPass& setFillMode(FillMode fillMode)
		{
			m_impl->setFillMode(fillMode);
			return *this;
		}
	};

	export std::unique_ptr<Shader> createShader(const ShaderDesc& desc);
	export std::unique_ptr<Pipeline> createPipeline(const Shader& shader,
													const PipelineDesc& desc = {});
	export std::unique_ptr<Material> createMaterial(const Pipeline& pipeline);
	export std::unique_ptr<RenderPass> createRenderPass(RenderSurface& surface);
} // namespace lune::gfx
