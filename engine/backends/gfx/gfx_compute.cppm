module;
#include <functional>
#include <map>
#include <memory>
#include <string>
export module lune.gfx:compute;

import :buffer;
import :texture;

namespace lune::gfx
{
	export class IComputeKernelImpl
	{
	protected:
		std::string m_name;

	public:
		explicit IComputeKernelImpl(const std::string& name) : m_name(name)
		{
		}

		virtual ~IComputeKernelImpl() = default;

		[[nodiscard]] const std::string& name() const noexcept
		{
			return m_name;
		}

		virtual void dispatch(size_t threadCount) = 0;
		virtual void dispatch(size_t x, size_t y, size_t z) = 0;
		virtual void dispatch(size_t x, size_t y, size_t z, std::function<void()> callback) = 0;

		virtual void setUniform(const std::string& name, const Buffer& buffer) = 0;
		virtual void setUniform(const std::string& name, const Texture& texture) = 0;
		virtual void setUniform(const std::string& name, const void* data, size_t size) = 0;

		virtual void waitUntilComplete() = 0;
	};

	export class ComputeKernel
	{
		std::unique_ptr<IComputeKernelImpl> m_impl;

	public:
		explicit ComputeKernel(std::unique_ptr<IComputeKernelImpl> impl) : m_impl(std::move(impl))
		{
		}

		~ComputeKernel() = default;

		[[nodiscard]] IComputeKernelImpl* getImpl() const
		{
			return m_impl.get();
		}

		ComputeKernel& dispatch(const size_t threadCount)
		{
			m_impl->dispatch(threadCount);
			return *this;
		}

		ComputeKernel& dispatch(const size_t x, const size_t y, const size_t z)
		{
			m_impl->dispatch(x, y, z);
			return *this;
		}

		ComputeKernel& dispatch(const size_t x, const size_t y, const size_t z,
								const std::function<void()>& callback)
		{
			m_impl->dispatch(x, y, z, callback);
			return *this;
		}

		ComputeKernel& setUniform(const std::string& name, const Buffer& buffer)
		{
			m_impl->setUniform(name, buffer);
			return *this;
		}

		ComputeKernel& setUniform(const std::string& name, const Texture& texture)
		{
			m_impl->setUniform(name, texture);
			return *this;
		}

		ComputeKernel& setUniform(const std::string& name, const void* data, size_t size)
		{
			m_impl->setUniform(name, data, size);
			return *this;
		}

		template <typename T> ComputeKernel& setUniform(const std::string& name, const T& value)
		{
			m_impl->setUniform(name, &value, sizeof(T));
			return *this;
		}

		ComputeKernel& waitUntilComplete()
		{
			m_impl->waitUntilComplete();
			return *this;
		}
	};


	export class IComputeShaderImpl
	{
	protected:
		std::map<std::string, std::unique_ptr<ComputeKernel>> m_kernels{};
		std::string m_path;

	public:
		explicit IComputeShaderImpl(const std::string& path) : m_path(path)
		{
		}

		virtual ~IComputeShaderImpl() = default;

		[[nodiscard]] const std::string& path() const noexcept
		{
			return m_path;
		}

		[[nodiscard]] ComputeKernel& kernel(const std::string& name) const
		{
			return *m_kernels.at(name);
		}

		[[nodiscard]] bool hasKernel(const std::string& name) const
		{
			return m_kernels.contains(name);
		}
	};


	export class ComputeShader
	{
		std::unique_ptr<IComputeShaderImpl> m_impl;

	public:
		explicit ComputeShader(std::unique_ptr<IComputeShaderImpl> impl) : m_impl(std::move(impl))
		{
		}

		~ComputeShader() = default;

		[[nodiscard]] IComputeShaderImpl* getImpl() const
		{
			return m_impl.get();
		}

		/**
		 * @brief Returns the ComputeKernel with the given name.
		 *
		 * @param name Name of the kernel.
		 * @return Reference to the ComputeKernel instance.
		 */
		[[nodiscard]] ComputeKernel& kernel(const std::string& name) const
		{
			return m_impl->kernel(name);
		}

		[[nodiscard]] bool hasKernel(const std::string& name) const
		{
			return m_impl->hasKernel(name);
		}
	};
} // namespace lune::gfx
