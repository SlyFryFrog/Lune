/**
* @note SIMD-specific implementations are separated into a source/implementation file
 *       to prevent breaking IntelliSense when parsing the module interface.
 */

module;
#include <algorithm>
#include <cmath>

#ifdef LUNE_USE_SIMD
#include <simd/vector_types.h>
#elif defined(LUNE_USE_GLM)
#warning LUNE_USE_GLM was declared, however this implementation is incomplete and may not work.
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#endif
export module lune:vector;

export namespace lune
{
	struct alignas(8) Vec2
	{
		float x, y;

		constexpr Vec2() noexcept :
			x(0),
			y(0)
		{
		}

		constexpr Vec2(const float X, const float Y) noexcept :
			x(X),
			y(Y)
		{
		}

		constexpr Vec2 operator+(const Vec2& o) const noexcept
		{
			return {x + o.x, y + o.y};
		}

		constexpr Vec2 operator-(const Vec2& o) const noexcept
		{
			return {x - o.x, y - o.y};
		}

		constexpr Vec2 operator*(const float s) const noexcept
		{
			return {x * s, y * s};
		}

		constexpr Vec2 operator/(const float s) const noexcept
		{
			return {x / s, y / s};
		}

		[[nodiscard]] constexpr float dot(const Vec2& o) const noexcept
		{
			return x * o.x + y * o.y;
		}

		[[nodiscard]] float length() const noexcept
		{
			return std::sqrt(dot(*this));
		}

		[[nodiscard]] Vec2 normalize() const noexcept
		{
			const float len = length();
			return len == 0 ? Vec2{} : *this / len;
		}

		[[nodiscard]] static constexpr Vec2 min(const Vec2& a, const Vec2& b) noexcept
		{
			return {std::min(a.x, b.x), std::min(a.y, b.y)};
		}

		[[nodiscard]] static constexpr Vec2 max(const Vec2& a, const Vec2& b) noexcept
		{
			return {std::max(a.x, b.x), std::max(a.y, b.y)};
		}

		[[nodiscard]] static constexpr Vec2 clamp(const Vec2& v, const Vec2& min,
		                                          const Vec2& max) noexcept
		{
			return {std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y)};
		}

		[[nodiscard]] static constexpr Vec2 lerp(const Vec2& a, const Vec2& b,
		                                         const float t) noexcept
		{
			return a + (b - a) * t;
		}

		[[nodiscard]] constexpr float distance(const Vec2& o) const noexcept
		{
			return (*this - o).length();
		}

#ifdef LUNE_USE_SIMD
		constexpr explicit Vec2(const simd::float2& v) noexcept;
		constexpr explicit operator simd::float2() const noexcept;
#endif

#ifdef LUNE_USE_GLM
		constexpr explicit Vec2(const glm::vec2& v) noexcept;
		constexpr operator glm::vec2() const noexcept;
#endif
	};


	struct alignas(16) Vec3
	{
		float x, y, z;

	private:
		float _pad = 0;	// Padding for 16-byte alignment

	public:
		constexpr Vec3() noexcept :
			x(0),
			y(0),
			z(0)
		{
		}

		constexpr Vec3(const float X, const float Y, const float Z) noexcept :
			x(X),
			y(Y),
			z(Z)
		{
		}

		constexpr Vec3 operator+(const Vec3& o) const noexcept
		{
			return {x + o.x, y + o.y, z + o.z};
		}

		constexpr Vec3 operator-(const Vec3& o) const noexcept
		{
			return {x - o.x, y - o.y, z - o.z};
		}

		constexpr Vec3 operator*(const float s) const noexcept
		{
			return {x * s, y * s, z * s};
		}

		constexpr Vec3 operator/(const float s) const noexcept
		{
			return {x / s, y / s, z / s};
		}

		[[nodiscard]] constexpr float dot(const Vec3& o) const noexcept
		{
			return x * o.x + y * o.y + z * o.z;
		}

		[[nodiscard]] constexpr Vec3 cross(const Vec3& o) const noexcept
		{
			return {y * o.z - z * o.y,
			        z * o.x - x * o.z,
			        x * o.y - y * o.x};
		}

		[[nodiscard]] float length() const noexcept
		{
			return std::sqrt(dot(*this));
		}

		[[nodiscard]] Vec3 normalize() const noexcept
		{
			const float len = length();
			return len == 0 ? Vec3{} : *this / len;
		}

		[[nodiscard]] static constexpr Vec3 min(const Vec3& a, const Vec3& b) noexcept
		{
			return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)};
		}

		[[nodiscard]] static constexpr Vec3 max(const Vec3& a, const Vec3& b) noexcept
		{
			return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)};
		}

		[[nodiscard]] static constexpr Vec3 clamp(const Vec3& v, const Vec3& min,
		                                          const Vec3& max) noexcept
		{
			return {std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y),
			        std::clamp(v.z, min.z, max.z)};
		}

		[[nodiscard]] static constexpr Vec3 lerp(const Vec3& a, const Vec3& b,
		                                         const float t) noexcept
		{
			return a + (b - a) * t;
		}

		[[nodiscard]] constexpr float distance(const Vec3& o) const noexcept
		{
			return (*this - o).length();
		}

		[[nodiscard]] constexpr Vec3 normalized() const noexcept
		{
			const float len = length();
			return (len > 0.0f)
				? Vec3{x / len, y / len, z / len}
				: Vec3{0.0f, 0.0f, 0.0f}; // Handle zero vector
		}

#ifdef LUNE_USE_SIMD
		constexpr explicit Vec3(const simd_float3& v) noexcept;
		constexpr explicit operator simd::float3() const noexcept;
#endif

#ifdef LUNE_USE_GLM
		constexpr explicit Vec3(const glm::vec3& v) noexcept;
		constexpr operator glm::vec3() const noexcept;
#endif
	};


	struct alignas(16) Vec4
	{
		float x, y, z, w;

		constexpr Vec4() noexcept :
			x(0),
			y(0),
			z(0),
			w(0)
		{
		}

		constexpr Vec4(const float X, const float Y, const float Z, const float W) noexcept :
			x(X),
			y(Y),
			z(Z),
			w(W)
		{
		}

		constexpr Vec4 operator+(const Vec4& o) const noexcept
		{
			return {x + o.x, y + o.y, z + o.z, w + o.w};
		}

		constexpr Vec4 operator-(const Vec4& o) const noexcept
		{
			return {x - o.x, y - o.y, z - o.z, w - o.w};
		}

		constexpr Vec4 operator*(const float s) const noexcept
		{
			return {x * s, y * s, z * s, w * s};
		}

		constexpr Vec4 operator/(const float s) const noexcept
		{
			return {x / s, y / s, z / s, w / s};
		}

		[[nodiscard]] constexpr float dot(const Vec4& o) const noexcept
		{
			return x * o.x + y * o.y + z * o.z + w * o.w;
		}

		[[nodiscard]] float length() const noexcept
		{
			return std::sqrt(dot(*this));
		}

		[[nodiscard]] Vec4 normalize() const noexcept
		{
			const float len = length();
			return len == 0 ? Vec4{} : *this / len;
		}

		[[nodiscard]] static constexpr Vec4 min(const Vec4& a, const Vec4& b) noexcept
		{
			return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w)};
		}

		[[nodiscard]] static constexpr Vec4 max(const Vec4& a, const Vec4& b) noexcept
		{
			return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w)};
		}

		[[nodiscard]] static constexpr Vec4 clamp(const Vec4& v, const Vec4& min,
		                                          const Vec4& max) noexcept
		{
			return {std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y),
			        std::clamp(v.z, min.z, max.z), std::clamp(v.w, min.w, max.w)};
		}

		[[nodiscard]] static constexpr Vec4 lerp(const Vec4& a, const Vec4& b,
		                                         const float t) noexcept
		{
			return a + (b - a) * t;
		}

		[[nodiscard]] constexpr float distance(const Vec4& o) const noexcept
		{
			return (*this - o).length();
		}

#ifdef LUNE_USE_SIMD
		constexpr explicit Vec4(const simd::float4& v) noexcept;
		constexpr explicit operator simd::float4() const noexcept;
#endif

#ifdef LUNE_USE_GLM
		constexpr explicit Vec4(const glm::vec4& v) noexcept;
		constexpr operator glm::vec4() const noexcept;
#endif
	};
}

